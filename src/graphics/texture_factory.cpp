#include "common.h"
#include "moth_graphics/graphics/texture_factory.h"
#include "moth_graphics/graphics/itexture.h"

//NOLINTBEGIN(readability-function-cognitive-complexity)

namespace moth_graphics::graphics {
    TextureFactory::TextureFactory(AssetContext& context)
        : m_context(context) {
    }

    void TextureFactory::FlushCache() {
        m_cachedTextures.clear();
        m_fallbackDesc.reset();
    }

    bool TextureFactory::LoadTexturePack(std::filesystem::path const& path) {
        auto const rootPath = path.parent_path();

        if (!std::filesystem::exists(path)) {
            spdlog::error("LoadTexturePack: '{}' does not exist", path.string());
            return false;
        }

        std::ifstream ifile(path);
        if (!ifile.is_open()) {
            spdlog::error("LoadTexturePack: failed to open '{}'", path.string());
            return false;
        }

        nlohmann::json json;
        try {
            ifile >> json;
        } catch (std::exception& e) {
            spdlog::error("LoadTexturePack: failed to parse '{}': {}", path.string(), e.what());
            return false;
        }

        if (!json.contains("atlases") || !json["atlases"].is_array()) {
            spdlog::error("LoadTexturePack: '{}' is missing or has invalid 'atlases' array", path.string());
            return false;
        }

        bool anyCached = false;
        for (auto&& atlasJson : json["atlases"]) {
            if (!atlasJson.is_object()) {
                spdlog::error("LoadTexturePack: '{}': atlas entry is not an object", path.string());
                continue;
            }
            if (!atlasJson.contains("atlas") || !atlasJson["atlas"].is_string()) {
                spdlog::error("LoadTexturePack: '{}': atlas entry missing 'atlas' string field", path.string());
                continue;
            }
            if (!atlasJson.contains("images") || !atlasJson["images"].is_array()) {
                spdlog::error("LoadTexturePack: '{}': atlas entry missing 'images' array", path.string());
                continue;
            }

            std::filesystem::path atlasRelPath;
            atlasJson.at("atlas").get_to(atlasRelPath);
            auto const atlasAbsPath = std::filesystem::absolute(rootPath / atlasRelPath).lexically_normal();

            auto texture = m_context.TextureFromFile(atlasAbsPath);
            if (!texture) {
                spdlog::error("LoadTexturePack: failed to load atlas texture '{}'", atlasAbsPath.string());
                continue;
            }
            auto sharedTexture = std::shared_ptr<ITexture>(std::move(texture));
            for (auto&& imageJson : atlasJson["images"]) {
                if (!imageJson.is_object()) {
                    spdlog::error("LoadTexturePack: '{}': image entry is not an object", atlasAbsPath.string());
                    continue;
                }
                if (!imageJson.contains("path") || !imageJson["path"].is_string()) {
                    spdlog::error("LoadTexturePack: '{}': image entry missing 'path' string field", atlasAbsPath.string());
                    continue;
                }
                if (!imageJson.contains("rect") || !imageJson["rect"].is_object()) {
                    spdlog::error("LoadTexturePack: '{}': image entry missing 'rect' object", atlasAbsPath.string());
                    continue;
                }
                try {
                    std::filesystem::path relPath;
                    imageJson.at("path").get_to(relPath);
                    auto const absPath = std::filesystem::absolute(rootPath / relPath).lexically_normal();
                    TextureDesc desc;
                    desc.m_texture = sharedTexture;
                    desc.m_path = absPath;
                    auto const& rectJson = imageJson.at("rect");
                    int const x = rectJson.at("x").get<int>();
                    int const y = rectJson.at("y").get<int>();
                    int const w = rectJson.at("w").get<int>();
                    int const h = rectJson.at("h").get<int>();
                    desc.m_sourceRect = moth_graphics::MakeRect(x, y, w, h);
                    m_cachedTextures.insert(std::make_pair(absPath.string(), desc));
                    anyCached = true;
                } catch (std::exception& e) {
                    spdlog::error("LoadTexturePack: '{}': failed to load image entry: {}", atlasAbsPath.string(), e.what());
                    continue;
                }
            }
        }

        return anyCached;
    }

    std::shared_ptr<ITexture> TextureFactory::GetTexture(std::filesystem::path const& path) {
        auto const key = std::filesystem::absolute(path).lexically_normal().string();
        auto const cacheIt = m_cachedTextures.find(key);
        if (std::end(m_cachedTextures) != cacheIt) {
            return cacheIt->second.m_texture;
        }
        if (std::shared_ptr<ITexture> texture = m_context.TextureFromFile(path)) {
            IntVec2 textureDimensions{ texture->GetWidth(), texture->GetHeight() };
            IntRect sourceRect{ { 0, 0 }, textureDimensions };
            TextureDesc cacheDesc;
            cacheDesc.m_path = key;
            cacheDesc.m_sourceRect = sourceRect;
            cacheDesc.m_texture = texture;
            m_cachedTextures.insert(std::make_pair(key, cacheDesc));
            return texture;
        }
        if (m_fallbackDesc) {
            spdlog::warn("TextureFactory: '{}' not found, using fallback", path.string());
            return m_fallbackDesc->m_texture;
        }
        spdlog::error("TextureFactory: '{}' not found and no fallback set", path.string());
        return nullptr;
    }

    IntRect TextureFactory::GetTextureRect(std::filesystem::path const& path) const {
        auto const key = std::filesystem::absolute(path).lexically_normal().string();
        auto const cacheIt = m_cachedTextures.find(key);
        if (std::end(m_cachedTextures) != cacheIt) {
            return cacheIt->second.m_sourceRect;
        }
        if (m_fallbackDesc) {
            return m_fallbackDesc->m_sourceRect;
        }
        return {};
    }

    void TextureFactory::SetFallbackTexture(std::shared_ptr<ITexture> texture) {
        if (!texture) {
            m_fallbackDesc.reset();
            return;
        }
        IntRect const sourceRect{ { 0, 0 }, { texture->GetWidth(), texture->GetHeight() } };
        m_fallbackDesc = TextureDesc{ texture, sourceRect, {} };
    }

    void TextureFactory::SetFallbackTexture(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) {
        if (!texture) {
            m_fallbackDesc.reset();
            return;
        }
        m_fallbackDesc = TextureDesc{ std::move(texture), sourceRect, {} };
    }
}

//NOLINTEND(readability-function-cognitive-complexity)
