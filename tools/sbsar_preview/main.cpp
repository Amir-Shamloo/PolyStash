// Renders a Substance .sbsar to uncompressed TGA maps so the PolyStash backend
// can turn them into PNG thumbnails and PBR maps for the 3D shader ball.
//
// Usage: sbsar_preview <input.sbsar> <output_dir>
//
// One <output-identifier>.tga file is written per image output. The backend
// converts them to PNG and exposes them through the `sbsar_textures` metadata.

#include <substance/framework/framework.h>
#include <substance/framework/outputopt.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace SubstanceAir;

namespace {

const int kOutputSizeLog2 = 10;  // 1024x1024, the closest power of two to 1080p

bool loadFile(const std::string& path, std::vector<unsigned char>& bytes) {
    std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
    if (!is) return false;
    is.seekg(0, std::ios::end);
    const std::streamoff size = is.tellg();
    if (size <= 0) return false;
    is.seekg(0, std::ios::beg);
    bytes.resize(static_cast<size_t>(size));
    is.read(reinterpret_cast<char*>(&bytes[0]), size);
    return true;
}

std::string sanitize(const std::string& identifier) {
    std::string safe;
    safe.reserve(identifier.size());
    for (const char c : identifier) {
        const bool alphanumeric = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        safe.push_back(alphanumeric || c == '_' || c == '-' ? c : '_');
    }
    return safe;
}

void setOutputSize(const GraphInstances& instances) {
    for (GraphInstances::const_iterator graph = instances.begin(); graph != instances.end(); ++graph) {
        const GraphInstance::Inputs& inputs = (*graph)->getInputs();
        for (GraphInstance::Inputs::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
            InputInstanceBase* input = *it;
            if (input->mDesc.mIdentifier == "$outputsize" &&
                input->mDesc.mType == Substance_IOType_Integer2) {
                static_cast<InputInstanceInt2*>(input)->setValue(Vec2Int(kOutputSizeLog2, kOutputSizeLog2));
            }
        }
    }
}

/// Write one rendered output as an uncompressed TGA. Grayscale outputs use
/// image type 3, colour outputs type 2 in BGRA order.
void writeTga(const std::string& path, const SubstanceTexture& texture) {
    const unsigned int width = texture.level0Width;
    const unsigned int height = texture.level0Height;
    const bool luminance = (texture.pixelFormat & ~Substance_PF_sRGB) == Substance_PF_L;

    std::vector<unsigned char> header(18, 0);
    header[2] = luminance ? 3 : 2;
    header[12] = static_cast<unsigned char>(width & 0xFF);
    header[13] = static_cast<unsigned char>((width >> 8) & 0xFF);
    header[14] = static_cast<unsigned char>(height & 0xFF);
    header[15] = static_cast<unsigned char>((height >> 8) & 0xFF);
    header[16] = luminance ? 8 : 32;
    header[17] = luminance ? 0x08 : 0x28;  // top-down, 8 alpha bits

    std::ofstream out(path.c_str(), std::ios::out | std::ios::binary);
    if (!out) return;
    out.write(reinterpret_cast<const char*>(header.data()), static_cast<std::streamsize>(header.size()));

    const unsigned char* src = static_cast<const unsigned char*>(texture.buffer);
    const size_t pixel_count = static_cast<size_t>(width) * height;
    if (luminance) {
        out.write(reinterpret_cast<const char*>(src), static_cast<std::streamsize>(pixel_count));
        return;
    }

    std::vector<unsigned char> row(width * 4);
    for (size_t i = 0; i < pixel_count; ++i) {
        const unsigned char* p = src + i * 4;
        unsigned char r, g, b, a;
        switch (texture.channelsOrder) {
            case Substance_ChanOrder_ABGR: a = p[0]; b = p[1]; g = p[2]; r = p[3]; break;
            case Substance_ChanOrder_ARGB: a = p[0]; r = p[1]; g = p[2]; b = p[3]; break;
            case Substance_ChanOrder_BGRA: b = p[0]; g = p[1]; r = p[2]; a = p[3]; break;
            default:                       r = p[0]; g = p[1]; b = p[2]; a = p[3]; break;
        }
        row[i * 4 + 0] = b;
        row[i * 4 + 1] = g;
        row[i * 4 + 2] = r;
        row[i * 4 + 3] = a;
    }
    out.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row.size()));
}

void saveOutputs(const GraphInstances& instances, const std::string& outputDir) {
    for (GraphInstances::const_iterator graph = instances.begin(); graph != instances.end(); ++graph) {
        const GraphInstance::Outputs& outputs = (*graph)->getOutputs();
        for (GraphInstance::Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
            OutputInstance::Result result((*it)->grabResult());
            if (result.get() == nullptr || !result->isImage()) continue;

            RenderResultImage* image = dynamic_cast<RenderResultImage*>(result.get());
            if (image == nullptr) continue;

            const std::string name = sanitize((*it)->mDesc.mIdentifier);
            writeTga(outputDir + "/" + name + ".tga", image->getTexture());
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.sbsar> <output_dir>\n";
        return 1;
    }
    const std::string inputPath = argv[1];
    const std::string outputDir = argv[2];

    std::vector<unsigned char> archive;
    if (!loadFile(inputPath, archive)) {
        std::cerr << "Unable to read " << inputPath << "\n";
        return 2;
    }

    // Uncompressed 8-bit output keeps both the writer and the Rust converter simple.
    OutputOptions outputOptions;
    outputOptions.mAllowedFormats = Format_RGBA8 | Format_L8;
    outputOptions.mMipmap = Mipmap_ForceNone;

    PackageDesc package(&archive[0], archive.size(), outputOptions);
    if (!package.isValid()) {
        std::cerr << "Invalid package\n";
        return 3;
    }

    GraphInstances instances;
    instantiate(instances, package);
    setOutputSize(instances);

    Renderer renderer;
    renderer.push(instances);
    renderer.run();

    saveOutputs(instances, outputDir);
    return 0;
}
