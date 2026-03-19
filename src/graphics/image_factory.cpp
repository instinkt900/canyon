#include "common.h"
#include "canyon/graphics/image_factory.h"
#include "canyon/graphics/itexture.h"

namespace canyon::graphics {
    ImageFactory::ImageFactory(AssetContext& context)
        : m_context(context) {
    }

    void ImageFactory::FlushCache() {
        m_cachedImages.clear();
    }

    bool ImageFactory::LoadTexturePack(std::filesystem::path const& path) {
        auto const rootPath = path.parent_path();
        auto const imagePath = path;
        auto detailsPath = path;
        detailsPath.replace_extension(".json");

        if (!std::filesystem::exists(imagePath) || !std::filesystem::exists(detailsPath)) {
            return false;
        }

        auto texture = m_context.TextureFromFile(imagePath);
        if (!texture) {
            return false;
        }
        auto sharedTexture = std::shared_ptr<ITexture>(texture.release());

        std::ifstream ifile(detailsPath);
        if (!ifile.is_open()) {
            return false;
        }

        nlohmann::json json;
        try {
            ifile >> json;
        } catch (std::exception&) {
            return false;
        }

        if (!json.contains("images") || !json["images"].is_array()) {
            return false;
        }

        for (auto&& imageJson : json["images"]) {
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
                desc.m_path = absPath.string();
                imageJson.at("rect").get_to(desc.m_sourceRect);
                m_cachedImages.insert(std::make_pair(desc.m_path.string(), desc));
            } catch (std::exception&) {
                continue;
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
        return nullptr;
    }
}

