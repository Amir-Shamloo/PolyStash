// PolyStash SBSAR renderer.
//
// Loads a Substance archive through the vendored Adobe Substance SDK, renders
// every output the graph exposes at a native 2048x2048 resolution, converts each
// result to 8-bit RGBA and writes one PNG named after its real identifier.
//
// Usage: sbsar_render.exe <input.sbsar> <output-directory>
//
// No upscaling and no filename parsing happen here: the SDK reports the exact
// output identifier and the exact output size.

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <substance/framework/framework.h>
#include <substance/pixelformat.h>
#include <substance/texture.h>

namespace
{

// $outputsize in an SBSAR is a size LOG2, not pixels (Autumn_Leaves default "9,9" = 512):
// 11 renders natively at 2048x2048.
constexpr int kOutputSizeLog2 = 11;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

bool read_file(const std::filesystem::path& path, std::vector<unsigned char>& data)
{
	std::ifstream stream(path, std::ios::binary | std::ios::ate);
	if (!stream)
		return false;

	const std::streamsize size = stream.tellg();
	if (size <= 0)
		return false;

	data.resize(static_cast<size_t>(size));
	stream.seekg(0, std::ios::beg);
	return static_cast<bool>(stream.read(reinterpret_cast<char*>(data.data()), size));
}

bool is_filename_char(unsigned char character)
{
	return (character >= 'a' && character <= 'z')
		|| (character >= 'A' && character <= 'Z')
		|| (character >= '0' && character <= '9')
		|| character == '_' || character == '-';
}

// Keep the identifier as authored, only replacing characters that cannot be
// used in a portable file name. Case is intentionally left untouched.
std::string sanitize_identifier(const char* identifier)
{
	std::string name;
	for (const unsigned char* cursor = reinterpret_cast<const unsigned char*>(identifier); *cursor != 0; ++cursor)
		name.push_back(is_filename_char(*cursor) ? static_cast<char>(*cursor) : '_');
	if (name.empty())
		return "output";
	return name;
}

std::string lower_identifier(const char* identifier)
{
	std::string lower;
	for (const unsigned char* cursor = reinterpret_cast<const unsigned char*>(identifier); *cursor != 0; ++cursor)
	{
		const unsigned char character = *cursor;
		const bool upper = character >= 'A' && character <= 'Z';
		lower.push_back(static_cast<char>(upper ? character - 'A' + 'a' : character));
	}
	return lower;
}

bool is_data_identifier(const char* identifier)
{
	static const char* const kDataTokens[] = {
		"normal", "bump", "rough", "metal", "height", "displacement",
		"opacity", "alpha", "occlusion", "ambientocclusion", "mask",
		"gloss", "smoothness"
	};

	const std::string lower = lower_identifier(identifier);
	for (const char* token : kDataTokens)
	{
		if (lower.find(token) != std::string::npos)
			return true;
	}
	return false;
}

float clamp01(float value)
{
	if (!(value > 0.0f)) // Also maps NaN to zero.
		return 0.0f;
	return value > 1.0f ? 1.0f : value;
}

float srgb_encode(float value)
{
	value = clamp01(value);
	constexpr float kLinearCutoff = 0.0031308f;
	if (value <= kLinearCutoff)
		return value * 12.92f;
	return 1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f;
}

unsigned char to_byte(float value)
{
	return static_cast<unsigned char>(std::lround(clamp01(value) * 255.0f));
}

float half_to_float(std::uint16_t half)
{
	const std::uint32_t sign = static_cast<std::uint32_t>(half & 0x8000u) << 16;
	int exponent = (half >> 10) & 0x1F;
	std::uint32_t mantissa = half & 0x03FFu;
	std::uint32_t bits = 0;

	if (exponent == 0)
	{
		if (mantissa != 0)
		{
			exponent = 1;
			while ((mantissa & 0x0400u) == 0)
			{
				mantissa <<= 1;
				--exponent;
			}
			mantissa &= 0x03FFu;
			bits = sign | (static_cast<std::uint32_t>(exponent + (127 - 15)) << 23) | (mantissa << 13);
		}
		else
		{
			bits = sign;
		}
	}
	else if (exponent == 0x1F)
	{
		bits = sign | 0x7F800000u | (mantissa << 13);
	}
	else
	{
		bits = sign | (static_cast<std::uint32_t>(exponent + (127 - 15)) << 23) | (mantissa << 13);
	}

	float value = 0.0f;
	std::memcpy(&value, &bits, sizeof(value));
	return value;
}

std::uint16_t read_u16(const unsigned char* source)
{
	std::uint16_t value = 0;
	std::memcpy(&value, source, sizeof(value));
	return value;
}

std::uint32_t read_u32(const unsigned char* source)
{
	std::uint32_t value = 0;
	std::memcpy(&value, source, sizeof(value));
	return value;
}

// ---------------------------------------------------------------------------
// Raw (uncompressed) surfaces
// ---------------------------------------------------------------------------

enum class SampleKind
{
	U8,
	U16,
	Half,
	Float
};

struct Surface
{
	const unsigned char* data = nullptr;
	int width = 0;
	int height = 0;
	unsigned pixel_format = 0;
	unsigned char channel_order = 0;
};

struct RawLayout
{
	int channels = 0;
	bool has_alpha = false;
	SampleKind kind = SampleKind::U8;
};

bool raw_channels(unsigned format, RawLayout& layout)
{
	switch (format & Substance_PF_MASK_RAWChannels)
	{
	case Substance_PF_RGBA:
		layout.channels = 4;
		layout.has_alpha = true;
		return true;
	case Substance_PF_RGBx:
		layout.channels = 4;
		layout.has_alpha = false;
		return true;
	case Substance_PF_RGB:
		layout.channels = 3;
		layout.has_alpha = false;
		return true;
	case Substance_PF_L:
		layout.channels = 1;
		layout.has_alpha = false;
		return true;
	default:
		return false;
	}
}

bool raw_precision(unsigned format, SampleKind& kind)
{
	switch (format & Substance_PF_MASK_RAWPrecision)
	{
	case 0:
		kind = SampleKind::U8;
		return true;
	case Substance_PF_16b:
		kind = SampleKind::U16;
		return true;
	case Substance_PF_FP:
		kind = SampleKind::Float;
		return true;
	case Substance_PF_16F:
		kind = SampleKind::Half;
		return true;
	default:
		return false;
	}
}

bool raw_layout(unsigned format, RawLayout& layout)
{
	return raw_channels(format, layout) && raw_precision(format, layout.kind);
}

template <SampleKind Kind>
float read_sample(const unsigned char* source, int index)
{
	if constexpr (Kind == SampleKind::U8)
	{
		return source[index] / 255.0f;
	}
	else if constexpr (Kind == SampleKind::U16)
	{
		return read_u16(source + index * 2) / 65535.0f;
	}
	else if constexpr (Kind == SampleKind::Half)
	{
		return half_to_float(read_u16(source + index * 2));
	}
	else
	{
		float value = 0.0f;
		std::memcpy(&value, source + index * 4, sizeof(value));
		return value;
	}
}

struct Rgba
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;
	float alpha = 1.0f;
};

unsigned channel_index(unsigned order, unsigned mask, unsigned shift)
{
	return (order & mask) >> shift;
}

template <SampleKind Kind>
Rgba read_rgb(const unsigned char* source, const Surface& surface, const RawLayout& layout)
{
	Rgba color;
	if constexpr (Kind == SampleKind::U8)
	{
		unsigned order = surface.channel_order;
		if (order == Substance_ChanOrder_NC)
			order = Substance_ChanOrder_RGBA;

		color.red = source[channel_index(order, Substance_ChanOrder_MASK_R, Substance_ChanOrder_RSHIFT_R)] / 255.0f;
		color.green = source[channel_index(order, Substance_ChanOrder_MASK_G, Substance_ChanOrder_RSHIFT_G)] / 255.0f;
		color.blue = source[channel_index(order, Substance_ChanOrder_MASK_B, Substance_ChanOrder_RSHIFT_B)] / 255.0f;
		color.alpha = layout.has_alpha
			? source[channel_index(order, Substance_ChanOrder_MASK_A, Substance_ChanOrder_RSHIFT_A)] / 255.0f
			: 1.0f;
	}
	else
	{
		color.red = read_sample<Kind>(source, 0);
		color.green = read_sample<Kind>(source, 1);
		color.blue = read_sample<Kind>(source, 2);
		color.alpha = layout.has_alpha ? read_sample<Kind>(source, 3) : 1.0f;
	}
	return color;
}

template <SampleKind Kind>
void convert_raw_pixels(const Surface& surface, const RawLayout& layout, bool color_output,
	std::vector<unsigned char>& rgba)
{
	const int bytes_per_sample = Kind == SampleKind::U8 ? 1 : (Kind == SampleKind::Float ? 4 : 2);
	const size_t pixel_count = static_cast<size_t>(surface.width) * static_cast<size_t>(surface.height);
	const size_t stride = static_cast<size_t>(layout.channels) * static_cast<size_t>(bytes_per_sample);
	// Floating point surfaces are linear by definition, so colour outputs need
	// the sRGB transfer applied here. Integer surfaces already carry the
	// encoding the graph authored, and data maps stay linear.
	const bool encode_srgb = (Kind == SampleKind::Half || Kind == SampleKind::Float) && color_output;

	for (size_t pixel = 0; pixel < pixel_count; ++pixel)
	{
		const unsigned char* source = surface.data + pixel * stride;
		Rgba color;
		if (layout.channels == 1)
		{
			color.red = color.green = color.blue = read_sample<Kind>(source, 0);
		}
		else
		{
			color = read_rgb<Kind>(source, surface, layout);
		}

		if (encode_srgb)
		{
			color.red = srgb_encode(color.red);
			color.green = srgb_encode(color.green);
			color.blue = srgb_encode(color.blue);
		}

		rgba[pixel * 4 + 0] = to_byte(color.red);
		rgba[pixel * 4 + 1] = to_byte(color.green);
		rgba[pixel * 4 + 2] = to_byte(color.blue);
		rgba[pixel * 4 + 3] = to_byte(color.alpha);
	}
}

bool convert_raw(const Surface& surface, unsigned format, bool color_output,
	std::vector<unsigned char>& rgba)
{
	RawLayout layout;
	if (!raw_layout(format, layout))
		return false;

	switch (layout.kind)
	{
	case SampleKind::U8:
		convert_raw_pixels<SampleKind::U8>(surface, layout, color_output, rgba);
		return true;
	case SampleKind::U16:
		convert_raw_pixels<SampleKind::U16>(surface, layout, color_output, rgba);
		return true;
	case SampleKind::Half:
		convert_raw_pixels<SampleKind::Half>(surface, layout, color_output, rgba);
		return true;
	case SampleKind::Float:
		convert_raw_pixels<SampleKind::Float>(surface, layout, color_output, rgba);
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// BC / DXT surfaces
// ---------------------------------------------------------------------------

struct Color
{
	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;
	unsigned char alpha = 255;
};

Color color_from_565(std::uint16_t value)
{
	const unsigned red = (value >> 11) & 0x1Fu;
	const unsigned green = (value >> 5) & 0x3Fu;
	const unsigned blue = value & 0x1Fu;
	Color color;
	color.red = static_cast<unsigned char>((red << 3) | (red >> 2));
	color.green = static_cast<unsigned char>((green << 2) | (green >> 4));
	color.blue = static_cast<unsigned char>((blue << 3) | (blue >> 2));
	return color;
}

Color mix_color(const Color& first, const Color& second, unsigned first_weight, unsigned second_weight)
{
	const unsigned total = first_weight + second_weight;
	Color color;
	color.red = static_cast<unsigned char>((first.red * first_weight + second.red * second_weight) / total);
	color.green = static_cast<unsigned char>((first.green * first_weight + second.green * second_weight) / total);
	color.blue = static_cast<unsigned char>((first.blue * first_weight + second.blue * second_weight) / total);
	return color;
}

void write_color(unsigned char* destination, const Color& color)
{
	destination[0] = color.red;
	destination[1] = color.green;
	destination[2] = color.blue;
	destination[3] = color.alpha;
}

void decode_bc1(const unsigned char* source, unsigned char* destination, bool allow_transparency)
{
	const std::uint16_t first = read_u16(source);
	const std::uint16_t second = read_u16(source + 2);
	const std::uint32_t indices = read_u32(source + 4);

	Color palette[4];
	palette[0] = color_from_565(first);
	palette[1] = color_from_565(second);

	if (first > second || !allow_transparency)
	{
		palette[2] = mix_color(palette[0], palette[1], 2, 1);
		palette[3] = mix_color(palette[0], palette[1], 1, 2);
	}
	else
	{
		palette[2] = mix_color(palette[0], palette[1], 1, 1);
		palette[3] = Color{0, 0, 0, 0};
	}

	for (int pixel = 0; pixel < 16; ++pixel)
	{
		const unsigned index = (indices >> (2 * pixel)) & 0x3u;
		write_color(destination + pixel * 4, palette[index]);
	}
}

void decode_alpha_block(const unsigned char* source, unsigned char* alpha)
{
	const unsigned char first = source[0];
	const unsigned char second = source[1];
	std::uint64_t indices = 0;
	for (int byte = 0; byte < 6; ++byte)
		indices |= static_cast<std::uint64_t>(source[2 + byte]) << (8 * byte);

	unsigned char palette[8];
	palette[0] = first;
	palette[1] = second;
	// six interpolated entries when first > second, four plus 0/255 otherwise
	const int steps = first > second ? 6 : 4;
	const int scale = first > second ? 7 : 5;
	for (int step = 1; step <= steps; ++step)
		palette[step + 1] = static_cast<unsigned char>(((scale - step) * first + step * second) / scale);
	if (first <= second)
	{
		palette[6] = 0;
		palette[7] = 255;
	}

	for (int pixel = 0; pixel < 16; ++pixel)
	{
		const unsigned index = static_cast<unsigned>((indices >> (3 * pixel)) & 0x7u);
		alpha[pixel] = palette[index];
	}
}

void decode_explicit_alpha(const unsigned char* source, unsigned char* alpha)
{
	for (int pixel = 0; pixel < 16; ++pixel)
	{
		const unsigned nibble = (source[pixel / 2] >> ((pixel & 1) * 4)) & 0x0Fu;
		alpha[pixel] = static_cast<unsigned char>(nibble * 17);
	}
}

void replace_alpha(unsigned char* destination, const unsigned char* alpha)
{
	for (int pixel = 0; pixel < 16; ++pixel)
		destination[pixel * 4 + 3] = alpha[pixel];
}

void decode_bc1_block(const unsigned char* source, unsigned char* destination)
{
	decode_bc1(source, destination, true);
}

void decode_bc2(const unsigned char* source, unsigned char* destination)
{
	unsigned char alpha[16];
	decode_explicit_alpha(source, alpha);
	decode_bc1(source + 8, destination, false);
	replace_alpha(destination, alpha);
}

void decode_bc3(const unsigned char* source, unsigned char* destination)
{
	unsigned char alpha[16];
	decode_alpha_block(source, alpha);
	decode_bc1(source + 8, destination, false);
	replace_alpha(destination, alpha);
}

void decode_bc4(const unsigned char* source, unsigned char* destination)
{
	unsigned char alpha[16];
	decode_alpha_block(source, alpha);
	for (int pixel = 0; pixel < 16; ++pixel)
	{
		destination[pixel * 4 + 0] = alpha[pixel];
		destination[pixel * 4 + 1] = alpha[pixel];
		destination[pixel * 4 + 2] = alpha[pixel];
		destination[pixel * 4 + 3] = 255;
	}
}

void decode_bc5(const unsigned char* source, unsigned char* destination)
{
	unsigned char red[16];
	unsigned char green[16];
	decode_alpha_block(source, red);
	decode_alpha_block(source + 8, green);
	for (int pixel = 0; pixel < 16; ++pixel)
	{
		destination[pixel * 4 + 0] = red[pixel];
		destination[pixel * 4 + 1] = green[pixel];
		destination[pixel * 4 + 2] = 0;
		destination[pixel * 4 + 3] = 255;
	}
}

struct BlockTarget
{
	const Surface& surface;
	std::vector<unsigned char>& rgba;
	int block_x = 0;
	int block_y = 0;
};

void blit_block(const unsigned char* block, const BlockTarget& target)
{
	const int rows = std::min(4, target.surface.height - target.block_y);
	const int columns = std::min(4, target.surface.width - target.block_x);
	for (int row = 0; row < rows; ++row)
	{
		const size_t offset = (static_cast<size_t>(target.block_y + row)
			* static_cast<size_t>(target.surface.width) + static_cast<size_t>(target.block_x)) * 4;
		unsigned char* destination = target.rgba.data() + offset;
		std::memcpy(destination, block + row * 16, static_cast<size_t>(columns) * 4);
	}
}

template <typename DecodeBlock>
void decode_compressed(const Surface& surface, int block_bytes,
	std::vector<unsigned char>& rgba, DecodeBlock decode)
{
	const unsigned char* source = surface.data;
	for (int block_y = 0; block_y < surface.height; block_y += 4)
	{
		for (int block_x = 0; block_x < surface.width; block_x += 4)
		{
			unsigned char block[64];
			decode(source, block);
			source += block_bytes;
			blit_block(block, BlockTarget{surface, rgba, block_x, block_y});
		}
	}
}

bool convert_compressed(const Surface& surface, unsigned format, std::vector<unsigned char>& rgba)
{
	switch (format)
	{
	case Substance_PF_BC1:
		decode_compressed(surface, 8, rgba, decode_bc1_block);
		return true;
	case Substance_PF_BC2:
		decode_compressed(surface, 16, rgba, decode_bc2);
		return true;
	case Substance_PF_BC3:
		decode_compressed(surface, 16, rgba, decode_bc3);
		return true;
	case Substance_PF_BC4:
		decode_compressed(surface, 8, rgba, decode_bc4);
		return true;
	case Substance_PF_BC5:
		decode_compressed(surface, 16, rgba, decode_bc5);
		return true;
	default:
		return false;
	}
}

// ---------------------------------------------------------------------------
// Surface dispatch
// ---------------------------------------------------------------------------

bool is_renderable(const Surface& surface)
{
	if (surface.data == nullptr)
		return false;
	if (surface.width <= 0)
		return false;
	return surface.height > 0;
}

bool convert_surface(const Surface& surface, bool color_output, std::vector<unsigned char>& rgba)
{
	if (!is_renderable(surface))
		return false;

	const size_t pixel_count = static_cast<size_t>(surface.width) * static_cast<size_t>(surface.height);
	rgba.assign(pixel_count * 4, 255);

	const unsigned format = surface.pixel_format & Substance_PF_MASK;
	switch (format & Substance_PF_MASK_RAWFormat)
	{
	case Substance_PF_RAW:
		return convert_raw(surface, format, color_output, rgba);
	case Substance_PF_BC:
		return convert_compressed(surface, format, rgba);
	default:
		return false;
	}
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void apply_native_output_size(SubstanceAir::GraphInstances& instances)
{
	for (const auto& instance : instances)
	{
		for (auto* input : instance->getInputs())
		{
			if (input->mDesc.mIdentifier != "$outputsize")
				continue;

			auto* size = dynamic_cast<SubstanceAir::InputInstanceInt2*>(input);
			if (size != nullptr)
				size->setValue(SubstanceAir::Vec2Int(kOutputSizeLog2, kOutputSizeLog2));
		}
	}
}

size_t render_output(SubstanceAir::OutputInstance& output, const std::filesystem::path& output_dir)
{
	if (!output.mDesc.isImage())
		return 0;

	auto result = output.grabResult();
	if (!result)
	{
		std::fprintf(stderr, "NO_RESULT %s\n", output.mDesc.mIdentifier.c_str());
		return 0;
	}

	auto* image = dynamic_cast<SubstanceAir::RenderResultImage*>(result.get());
	if (image == nullptr)
	{
		std::fprintf(stderr, "NOT_IMAGE %s\n", output.mDesc.mIdentifier.c_str());
		return 0;
	}

	const SubstanceTexture& texture = image->getTexture();
	Surface surface;
	surface.data = static_cast<const unsigned char*>(texture.buffer);
	surface.width = texture.level0Width;
	surface.height = texture.level0Height;
	surface.pixel_format = texture.pixelFormat;
	surface.channel_order = texture.channelsOrder;

	const bool color_output = !is_data_identifier(output.mDesc.mIdentifier.c_str());
	std::vector<unsigned char> rgba;
	if (!convert_surface(surface, color_output, rgba))
	{
		std::fprintf(stderr, "UNSUPPORTED pf=%u\n", texture.pixelFormat);
		return 0;
	}

	const std::string identifier = sanitize_identifier(output.mDesc.mIdentifier.c_str());
	const std::filesystem::path path = output_dir / (identifier + ".png");
	const int stride = surface.width * 4;
	if (stbi_write_png(path.string().c_str(), surface.width, surface.height, 4, rgba.data(), stride) == 0)
	{
		std::fprintf(stderr, "WRITE_FAILED %s\n", identifier.c_str());
		return 0;
	}

	std::fprintf(stderr, "OUT %s %dx%d pf=%u\n", output.mDesc.mIdentifier.c_str(),
		surface.width, surface.height, texture.pixelFormat);
	return 1;
}

size_t render_graph(SubstanceAir::GraphInstance& graph, const std::filesystem::path& output_dir)
{
	size_t written = 0;
	for (auto* output : graph.getOutputs())
		written += render_output(*output, output_dir);
	return written;
}

} // namespace

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::fprintf(stderr, "usage: sbsar_render <input.sbsar> <output-directory>\n");
		return 2;
	}

	const std::filesystem::path source = argv[1];
	const std::filesystem::path output_dir = argv[2];

	std::error_code error;
	std::filesystem::create_directories(output_dir, error);
	if (error)
	{
		std::fprintf(stderr, "cannot create output directory %s\n", output_dir.string().c_str());
		return 2;
	}

	std::vector<unsigned char> data;
	if (!read_file(source, data))
	{
		std::fprintf(stderr, "cannot read %s\n", source.string().c_str());
		return 2;
	}

	SubstanceAir::PackageDesc package(data.data(), data.size());
	if (!package.isValid())
	{
		std::fprintf(stderr, "invalid SBSAR %s\n", source.string().c_str());
		return 2;
	}

	SubstanceAir::GraphInstances instances;
	SubstanceAir::instantiate(instances, package);
	apply_native_output_size(instances);

	// Render one graph at a time so each graph's PNGs are on disk before the
	// next job starts: a timeout leaves the completed graphs behind.
	SubstanceAir::Renderer renderer;
	size_t rendered = 0;
	for (const auto& instance : instances)
	{
		renderer.push(*instance);
		renderer.run();
		rendered += render_graph(*instance, output_dir);
	}

	std::fprintf(stderr, "RENDERED %zu\n", rendered);
	return rendered > 0 ? 0 : 1;
}
