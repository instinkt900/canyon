#include "common.h"
#include "moth_graphics/graphics/image_factory.h"
#include "moth_graphics/graphics/itexture.h"

//NOLINTBEGIN(readability-function-cognitive-complexity)

namespace moth_graphics::graphics {
    ImageFactory::ImageFactory(AssetContext& context)
        : m_context(context) {
    }

    void ImageFactory::FlushCache() {
        m_cachedImages.clear();
    }

    bool ImageFactory::LoadTexturePack(std::filesystem::path const& path) {
        auto const rootPath = path.parent_path();

        if (!std::filesystem::exists(path)) {
            return false;
        }

        std::ifstream ifile(path);
        if (!ifile.is_open()) {
            return false;
        }

        nlohmann::json json;
        try {
            ifile >> json;
        } catch (std::exception&) {
            return false;
        }

        if (!json.contains("atlases") || !json["atlases"].is_array()) {
            return false;
        }

        for (auto&& atlasJson : json["atlases"]) {
            if (!atlasJson.is_object()) {
                continue;
            }
            if (!atlasJson.contains("atlas") || !atlasJson["atlas"].is_string()) {
                continue;
            }
            if (!atlasJson.contains("images") || !atlasJson["images"].is_array()) {
                continue;
            }

            std::filesystem::path atlasRelPath;
            atlasJson.at("atlas").get_to(atlasRelPath);
            auto const atlasAbsPath = std::filesystem::absolute(rootPath / atlasRelPath).lexically_normal();

            auto texture = m_context.TextureFromFile(atlasAbsPath);
            if (!texture) {
                continue;
            }
            auto sharedTexture = std::shared_ptr<ITexture>(texture.release());

            for (auto&& imageJson : atlasJson["images"]) {
                if (!imageJson.is_object()) {
                    continue;
                }
                if (!imageJson.contains("path") || !imageJson["path"].is_string()) {
                    continue;
                }
                if (!imageJson.contains("rect") || !imageJson["rect"].is_object()) {
                    continue;
                }
                try {
                    std::filesystem::path relPath;
                    imageJson.at("path").get_to(relPath);
                    auto const absPath = std::filesystem::absolute(rootPath / relPath).lexically_normal();
                    ImageDesc desc;
                    desc.m_texture = sharedTexture;
                    desc.m_path = absPath;
                    auto const& rectJson = imageJson.at("rect");
                    int const x = rectJson.at("x").get<int>();
                    int const y = rectJson.at("y").get<int>();
                    int const w = rectJson.at("w").get<int>();
                    int const h = rectJson.at("h").get<int>();
                    desc.m_sourceRect = moth_graphics::MakeRect(x, y, w, h);
                    m_cachedImages.insert(std::make_pair(absPath.string(), desc));
                } catch (std::exception& e) {
                    continue;
                }
            }
        }

        return true;
    }

    std::unique_ptr<IImage> ImageFactory::GetImage(std::filesystem::path const& path) {
        auto const key = std::filesystem::absolute(path).lexically_normal().string();
        auto const cacheIt = m_cachedImages.find(key);
        if (std::end(m_cachedImages) != cacheIt) {
            auto const& imageDesc = cacheIt->second;
            return m_context.NewImage(imageDesc.m_texture, imageDesc.m_sourceRect);
        }
        if (std::shared_ptr<ITexture> texture = m_context.TextureFromFile(path)) {
            IntVec2 textureDimensions{ texture->GetWidth(), texture->GetHeight() };
            IntRect sourceRect{ { 0, 0 }, textureDimensions };
            ImageDesc cacheDesc;
            cacheDesc.m_path = key;
            cacheDesc.m_sourceRect = sourceRect;
            cacheDesc.m_texture = texture;
            m_cachedImages.insert(std::make_pair(key, cacheDesc));
            return m_context.NewImage(texture, sourceRect);
        }
        if (m_fallbackDesc) {
            spdlog::warn("ImageFactory: '{}' not found, using fallback", path.string());
            return m_context.NewImage(m_fallbackDesc->m_texture, m_fallbackDesc->m_sourceRect);
        }
        spdlog::error("ImageFactory: '{}' not found and no fallback set", path.string());
        return nullptr;
    }

    void ImageFactory::SetFallbackImage(std::shared_ptr<ITexture> texture) {
        if (!texture) {
            m_fallbackDesc.reset();
            return;
        }
        IntRect const sourceRect{ { 0, 0 }, { texture->GetWidth(), texture->GetHeight() } };
        m_fallbackDesc = ImageDesc{ texture, sourceRect, {} };
    }
}

//NOLINTEND(readability-function-cognitive-complexity)

