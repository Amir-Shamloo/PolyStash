// PolyStash C4D preview helper.
//
// Loads a Cinema 4D scene through the vendored Maxon Cineware SDK and writes a
// self-contained GLB (binary glTF 2.0) with one mesh per polygon object and a
// PBR base-colour material per scene material.
//
// Usage: c4d_preview.exe <input.c4d> <output.glb>
//
// The SDK has no polygon cache builder of its own: generators such as cubes or
// sweeps can only be exported when the .c4d file was saved with "Save Polygons
// for Cineware". Plain polygon objects always export.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// The Cineware headers disable C++ exceptions for their own code, which breaks
// every MSVC STL header included after them. Restore the setting once parsed.
#pragma push_macro("_HAS_EXCEPTIONS")
#include "c4d.h"
#include "default_alien_overloads.h"
#include "parameter_ids/material/mmaterial.h"
#pragma pop_macro("_HAS_EXCEPTIONS")

using namespace cineware;

namespace
{

struct MaterialData
{
	std::string name;
	float r = 0.8f;
	float g = 0.8f;
	float b = 0.8f;
	float roughness = 0.5f;
	float metallic = 0.0f;
};

struct Primitive
{
	std::string name;
	std::vector<float> positions; // x, y, z in glTF space
	std::vector<unsigned int> indices;
	int material = -1;
};

std::vector<MaterialData> g_materials;
std::unordered_map<const BaseMaterial*, int> g_material_lookup;
std::vector<Primitive> g_primitives;

std::string to_utf8(const String& value)
{
	// glTF JSON must be UTF-8; the default XBIT encoding is the OS code page.
	Char* raw = value.GetCStringCopy(STRINGENCODING_UTF8);
	std::string result = raw ? raw : "";
	DeleteMem(raw);
	return result;
}

// `Filename(const Char*)` decodes its input as the OS code page, which loses
// characters outside it. Wide paths therefore go through `String` as explicit
// UTF-8 instead.
Filename to_filename(const std::filesystem::path& path)
{
	const std::string utf8 = path.u8string();
	return Filename(String(utf8.c_str(), STRINGENCODING_UTF8));
}

float clamp01(float value)
{
	if (value < 0.0f)
		return 0.0f;
	if (value > 1.0f)
		return 1.0f;
	return value;
}

MaterialData read_material(BaseMaterial* material)
{
	MaterialData data;
	data.name = to_utf8(material->GetName());

	GeData value;
	if (material->GetParameter(MATERIAL_COLOR_COLOR, value))
	{
		const Vector color = value.GetVector();
		data.r = clamp01(color.x);
		data.g = clamp01(color.y);
		data.b = clamp01(color.z);
	}

	if (material->GetParameter(MATERIAL_COLOR_ROUGHNESS, value))
	{
		float roughness = (float)value.GetFloat();
		// Older scenes store the roughness as a percentage.
		if (roughness > 1.0f)
			roughness /= 100.0f;
		data.roughness = clamp01(roughness);
	}

	return data;
}

int material_index_for(BaseMaterial* material)
{
	if (!material)
		return -1;

	const auto found = g_material_lookup.find(material);
	if (found != g_material_lookup.end())
		return found->second;

	const int index = (int)g_materials.size();
	g_materials.push_back(read_material(material));
	g_material_lookup[material] = index;
	return index;
}

bool valid_index(Int32 index, Int32 count)
{
	return index >= 0 && index < count;
}

bool is_degenerate(int a, int b, int c)
{
	if (a == b)
		return true;
	return b == c || a == c;
}

void add_triangle(Primitive& primitive, int a, int b, int c)
{
	if (is_degenerate(a, b, c))
		return;

	// C4D is left-handed while glTF is right-handed. Z is negated above, which
	// mirrors the winding, so every face is emitted in reverse order.
	primitive.indices.push_back((unsigned int)a);
	primitive.indices.push_back((unsigned int)c);
	primitive.indices.push_back((unsigned int)b);
}

void fill_positions(Primitive& primitive, const Vector* points, Int32 point_count, const Matrix& transform)
{
	primitive.positions.resize((size_t)point_count * 3);
	for (Int32 i = 0; i < point_count; ++i)
	{
		const Vector world = transform * points[i];
		primitive.positions[(size_t)i * 3 + 0] = world.x;
		primitive.positions[(size_t)i * 3 + 1] = world.y;
		primitive.positions[(size_t)i * 3 + 2] = -world.z;
	}
}

bool has_valid_corners(int a, int b, int c, Int32 point_count)
{
	return valid_index(a, point_count) && valid_index(b, point_count) && valid_index(c, point_count);
}

void fill_indices(Primitive& primitive, const CPolygon* polygons, Int32 polygon_count, Int32 point_count)
{
	for (Int32 i = 0; i < polygon_count; ++i)
	{
		const CPolygon& polygon = polygons[i];
		const int a = polygon.a;
		const int b = polygon.b;
		const int c = polygon.c;
		const int d = polygon.d;

		if (!has_valid_corners(a, b, c, point_count))
			continue;

		add_triangle(primitive, a, b, c);

		if (d != c && valid_index(d, point_count))
			add_triangle(primitive, a, c, d);
	}
}

int material_from_tag(BaseTag* texture_tag)
{
	if (!texture_tag)
		return -1;

	GeData value;
	if (!texture_tag->GetParameter(TEXTURETAG_MATERIAL, value))
		return -1;

	return material_index_for((BaseMaterial*)value.GetLink());
}

bool has_geometry(const Vector* points, const CPolygon* polygons, Int32 point_count, Int32 polygon_count)
{
	if (!points || !polygons)
		return false;
	return point_count > 0 && polygon_count > 0;
}

void emit_polygon(PolygonObject* object, const Matrix& transform, int inherited_material)
{
	const Vector* points = object->GetPointR();
	const CPolygon* polygons = object->GetPolygonR();
	const Int32 point_count = object->GetPointCount();
	const Int32 polygon_count = object->GetPolygonCount();

	if (!has_geometry(points, polygons, point_count, polygon_count))
		return;

	Primitive primitive;
	primitive.name = to_utf8(object->GetName());
	fill_positions(primitive, points, point_count, transform);
	primitive.material = material_from_tag(object->GetTag(Ttexture));
	// Cache objects are geometry-only and carry no texture tag of their own.
	if (primitive.material < 0)
		primitive.material = inherited_material;
	fill_indices(primitive, polygons, polygon_count, point_count);

	if (!primitive.indices.empty())
		g_primitives.push_back(std::move(primitive));
}

// Mirrors the cache recursion documented on BaseObject::GetCache(): prefer the
// deform cache, fall back to the generator cache, and only emit actual polygon
// objects that are not generator inputs.
//
// World matrices are accumulated from GetMl() instead of calling GetMg():
// cache objects are virtual and not attached to a document, which makes GetMg()
// invalid for them.
void walk_object(BaseObject* object, const Matrix& parent_world, int inherited_material);

void emit_cache_polygons(BaseObject* cache, const Matrix& world, int material)
{
	for (BaseObject* node = cache; node; node = node->GetNext())
	{
		if (!node->GetBit(BIT_CONTROLOBJECT) && node->IsInstanceOf(Opolygon))
			emit_polygon((PolygonObject*)node, world, material);
	}
}

void walk_cache(BaseObject* object, const Matrix& world, int material)
{
	BaseObject* cache = object->GetCache();
	if (!cache)
	{
		if (!object->GetBit(BIT_CONTROLOBJECT) && object->IsInstanceOf(Opolygon))
			emit_polygon((PolygonObject*)object, world, material);
		return;
	}

	// Cache hierarchies live in the generator's local space; the cache
	// objects' own GetMl() values carry their internal transforms.
	for (BaseObject* node = cache; node; node = node->GetNext())
		walk_object(node, world, material);
}

void walk_object(BaseObject* object, const Matrix& parent_world, int inherited_material)
{
	if (!object)
		return;

	const Matrix world = parent_world * object->GetMl();
	int material = material_from_tag(object->GetTag(Ttexture));
	if (material < 0)
		material = inherited_material;

	BaseObject* deform_cache = object->GetDeformCache();
	if (deform_cache)
	{
		// A deform cache replaces the source object's geometry. Its points are
		// already deformed in the source object's local space, so `world` is
		// final and the cache object's own matrix must not be applied again.
		emit_cache_polygons(deform_cache, world, material);
	}
	else
	{
		walk_cache(object, world, material);
	}

	for (BaseObject* child = object->GetDown(); child; child = child->GetNext())
		walk_object(child, world, material);
}

void append_escaped_char(std::string& out, char ch)
{
	if (ch == '"' || ch == '\\')
	{
		out.push_back('\\');
		out.push_back(ch);
		return;
	}

	static const std::pair<char, const char*> escapes[] = {
		{'\b', "\\b"}, {'\f', "\\f"}, {'\n', "\\n"}, {'\r', "\\r"}, {'\t', "\\t"},
	};
	for (const auto& escape : escapes)
	{
		if (ch == escape.first)
		{
			out += escape.second;
			return;
		}
	}

	if ((unsigned char)ch < 0x20)
	{
		char buffer[7];
		std::snprintf(buffer, sizeof(buffer), "\\u%04x", (unsigned char)ch);
		out += buffer;
		return;
	}

	out.push_back(ch);
}

std::string json_string(const std::string& value)
{
	std::string out;
	out.reserve(value.size() + 2);
	out.push_back('"');
	for (const char ch : value)
		append_escaped_char(out, ch);
	out.push_back('"');
	return out;
}

std::string json_float(float value)
{
	if (!std::isfinite(value))
		value = 0.0f;

	std::ostringstream stream;
	stream.precision(9);
	stream << value;
	return stream.str();
}

void append_bytes(std::vector<unsigned char>& buffer, const void* data, size_t size)
{
	const unsigned char* bytes = (const unsigned char*)data;
	buffer.insert(buffer.end(), bytes, bytes + size);
}

void align_buffer(std::vector<unsigned char>& buffer)
{
	while (buffer.size() % 4 != 0)
		buffer.push_back(0);
}

void write_u32(std::ofstream& out, unsigned int value)
{
	unsigned char bytes[4] = {
		(unsigned char)(value & 0xFF),
		(unsigned char)((value >> 8) & 0xFF),
		(unsigned char)((value >> 16) & 0xFF),
		(unsigned char)((value >> 24) & 0xFF),
	};
	out.write((const char*)bytes, 4);
}

struct BufferView
{
	size_t offset = 0;
	size_t length = 0;
	int target = 0;
};

struct Accessor
{
	int buffer_view = 0;
	size_t count = 0;
	int component_type = 0;
	const char* type = "SCALAR";
	bool has_bounds = false;
	float minimum[3] = {0.0f, 0.0f, 0.0f};
	float maximum[3] = {0.0f, 0.0f, 0.0f};
};

struct PrimitiveInfo
{
	int position_accessor = 0;
	int index_accessor = 0;
	int material = -1;
	std::string name;
};

struct GlbModel
{
	std::vector<unsigned char> binary;
	std::vector<BufferView> views;
	std::vector<Accessor> accessors;
	std::vector<PrimitiveInfo> infos;
};

void append_position_accessor(GlbModel& model, const Primitive& primitive)
{
	align_buffer(model.binary);
	BufferView view;
	view.offset = model.binary.size();
	view.length = primitive.positions.size() * sizeof(float);
	view.target = 34962; // ARRAY_BUFFER
	append_bytes(model.binary, primitive.positions.data(), view.length);
	model.views.push_back(view);

	Accessor accessor;
	accessor.buffer_view = (int)model.views.size() - 1;
	accessor.count = primitive.positions.size() / 3;
	accessor.component_type = 5126; // FLOAT
	accessor.type = "VEC3";
	accessor.has_bounds = true;
	for (int axis = 0; axis < 3; ++axis)
		accessor.minimum[axis] = accessor.maximum[axis] = primitive.positions[axis];

	for (size_t i = 3; i < primitive.positions.size(); i += 3)
	{
		for (int axis = 0; axis < 3; ++axis)
		{
			const float value = primitive.positions[i + axis];
			accessor.minimum[axis] = std::min(accessor.minimum[axis], value);
			accessor.maximum[axis] = std::max(accessor.maximum[axis], value);
		}
	}

	model.accessors.push_back(accessor);
}

void append_index_accessor(GlbModel& model, const Primitive& primitive)
{
	align_buffer(model.binary);
	BufferView view;
	view.offset = model.binary.size();
	view.length = primitive.indices.size() * sizeof(unsigned int);
	view.target = 34963; // ELEMENT_ARRAY_BUFFER
	append_bytes(model.binary, primitive.indices.data(), view.length);
	model.views.push_back(view);

	Accessor accessor;
	accessor.buffer_view = (int)model.views.size() - 1;
	accessor.count = primitive.indices.size();
	accessor.component_type = 5125; // UNSIGNED_INT
	accessor.type = "SCALAR";
	model.accessors.push_back(accessor);
}

GlbModel build_glb_model(const std::vector<Primitive>& primitives)
{
	GlbModel model;
	for (const Primitive& primitive : primitives)
	{
		append_position_accessor(model, primitive);
		append_index_accessor(model, primitive);

		PrimitiveInfo info;
		info.position_accessor = (int)model.accessors.size() - 2;
		info.index_accessor = (int)model.accessors.size() - 1;
		info.material = primitive.material;
		info.name = primitive.name;
		model.infos.push_back(info);
	}
	return model;
}

void append_scene_indices(std::ostringstream& json, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		if (i)
			json << ',';
		json << i;
	}
}

void append_nodes_json(std::ostringstream& json, const GlbModel& model)
{
	json << "]}],\"nodes\":[";
	for (size_t i = 0; i < model.infos.size(); ++i)
	{
		if (i)
			json << ',';
		json << "{\"name\":" << json_string(model.infos[i].name) << ",\"mesh\":" << i << "}";
	}
}

void append_meshes_json(std::ostringstream& json, const GlbModel& model)
{
	json << "],\"meshes\":[";
	for (size_t i = 0; i < model.infos.size(); ++i)
	{
		const PrimitiveInfo& info = model.infos[i];
		if (i)
			json << ',';
		json << "{\"name\":" << json_string(info.name) << ",\"primitives\":[{\"attributes\":{\"POSITION\":"
		     << info.position_accessor << "},\"indices\":" << info.index_accessor << ",\"mode\":4";
		if (info.material >= 0)
			json << ",\"material\":" << info.material;
		json << "}]}";
	}
}

void append_materials_json(std::ostringstream& json, const std::vector<MaterialData>& materials)
{
	if (materials.empty())
		return;

	json << "],\"materials\":[";
	for (size_t i = 0; i < materials.size(); ++i)
	{
		const MaterialData& material = materials[i];
		if (i)
			json << ',';
		json << "{\"name\":" << json_string(material.name)
		     << ",\"doubleSided\":true,\"pbrMetallicRoughness\":{\"baseColorFactor\":["
		     << json_float(material.r) << ',' << json_float(material.g) << ',' << json_float(material.b)
		     << ",1],\"metallicFactor\":" << json_float(material.metallic)
		     << ",\"roughnessFactor\":" << json_float(material.roughness) << "}}";
	}
}

void append_accessors_json(std::ostringstream& json, const GlbModel& model)
{
	json << "],\"accessors\":[";
	for (size_t i = 0; i < model.accessors.size(); ++i)
	{
		const Accessor& accessor = model.accessors[i];
		if (i)
			json << ',';
		json << "{\"bufferView\":" << accessor.buffer_view << ",\"componentType\":" << accessor.component_type
		     << ",\"count\":" << accessor.count << ",\"type\":\"" << accessor.type << '\"';
		if (accessor.has_bounds)
		{
			json << ",\"min\":[" << json_float(accessor.minimum[0]) << ',' << json_float(accessor.minimum[1])
			     << ',' << json_float(accessor.minimum[2]) << "],\"max\":[" << json_float(accessor.maximum[0])
			     << ',' << json_float(accessor.maximum[1]) << ',' << json_float(accessor.maximum[2]) << ']';
		}
		json << '}';
	}
}

void append_buffer_views_json(std::ostringstream& json, const GlbModel& model)
{
	json << "],\"bufferViews\":[";
	for (size_t i = 0; i < model.views.size(); ++i)
	{
		if (i)
			json << ',';
		json << "{\"buffer\":0,\"byteOffset\":" << model.views[i].offset
		     << ",\"byteLength\":" << model.views[i].length << ",\"target\":" << model.views[i].target << '}';
	}
}

std::string build_glb_json(const GlbModel& model, const std::vector<MaterialData>& materials)
{
	std::ostringstream json;
	json << "{\"asset\":{\"version\":\"2.0\",\"generator\":\"PolyStash c4d_preview\"},\"scene\":0,\"scenes\":[{\"nodes\":[";
	append_scene_indices(json, model.infos.size());
	append_nodes_json(json, model);
	append_meshes_json(json, model);
	append_materials_json(json, materials);
	append_accessors_json(json, model);
	append_buffer_views_json(json, model);
	json << "],\"buffers\":[{\"byteLength\":" << model.binary.size() << "}]}";

	std::string json_bytes = json.str();
	while (json_bytes.size() % 4 != 0)
		json_bytes.push_back(' ');
	return json_bytes;
}

bool write_glb_file(const std::filesystem::path& path, const std::string& json, const std::vector<unsigned char>& binary)
{
	std::vector<unsigned char> padded_binary = binary;
	while (padded_binary.size() % 4 != 0)
		padded_binary.push_back(0);

	const unsigned int total_length = (unsigned int)(12 + 8 + json.size() + 8 + padded_binary.size());

	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	if (!out)
		return false;

	write_u32(out, 0x46546C67); // "glTF"
	write_u32(out, 2);
	write_u32(out, total_length);

	write_u32(out, (unsigned int)json.size());
	write_u32(out, 0x4E4F534A); // "JSON"
	out.write(json.data(), (std::streamsize)json.size());

	write_u32(out, (unsigned int)padded_binary.size());
	write_u32(out, 0x004E4942); // "BIN\0"
	if (!padded_binary.empty())
		out.write((const char*)padded_binary.data(), (std::streamsize)padded_binary.size());

	// Flush before checking: the destructor runs after this returns.
	out.flush();
	return out.good();
}

bool write_glb(const std::filesystem::path& path, const std::vector<Primitive>& primitives, const std::vector<MaterialData>& materials)
{
	GlbModel model = build_glb_model(primitives);
	return write_glb_file(path, build_glb_json(model, materials), model.binary);
}

bool export_scene(BaseDocument* document, const std::filesystem::path& output)
{
	for (BaseMaterial* material = document->GetFirstMaterial(); material; material = material->GetNext())
	{
		const int index = (int)g_materials.size();
		g_materials.push_back(read_material(material));
		g_material_lookup[material] = index;
	}

	const Matrix identity;
	for (BaseObject* object = document->GetFirstObject(); object; object = object->GetNext())
		walk_object(object, identity, -1);

	if (g_primitives.empty())
	{
		std::fprintf(stderr, "c4d_preview: no polygon geometry found (were polygon caches saved?)\n");
		return false;
	}

	std::fprintf(stderr, "c4d_preview: exporting %zu mesh(es), %zu material(s)\n", g_primitives.size(),
							 g_materials.size());
	return write_glb(output, g_primitives, g_materials);
}

} // namespace

// The Cineware SDK calls this to identify the host application in written files.
void GetWriterInfo(cineware::Int32& id, cineware::String& appname)
{
	id = 0;
	appname = "PolyStash C4D Preview";
}

// Windows entry point: the CRT hands `wmain` the wide command line
// (GetCommandLineW), so library paths outside the active code page survive.
// `main(int, char**)` would receive ANSI arguments and mangle them.
int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
	{
		std::fprintf(stderr, "usage: c4d_preview.exe <input.c4d> <output.glb>\n");
		return 2;
	}

	const std::filesystem::path input = argv[1];
	const std::filesystem::path output = argv[2];

	const Filename input_name = to_filename(input);
	if (!GeFExist(input_name))
	{
		std::fprintf(stderr, "c4d_preview: input does not exist: %s\n", input.u8string().c_str());
		return 3;
	}

	BaseDocument* document = LoadDocument(input_name, SCENEFILTER_OBJECTS | SCENEFILTER_MATERIALS);
	if (!document)
	{
		std::fprintf(stderr, "c4d_preview: could not read Cinema 4D scene: %s\n", input.u8string().c_str());
		return 3;
	}

	if (!document->HasCaches())
		std::fprintf(stderr, "c4d_preview: scene was saved without polygon caches; generator objects may be missing\n");

	const bool ok = export_scene(document, output);
	BaseDocument::Free(document);
	return ok ? 0 : 4;
}
