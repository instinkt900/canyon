#include "graphics/image_factory.h"
#include "graphics/itexture.h"
#include "utils/rect_serialization.h"

namespace graphics {
    ImageFactory::ImageFactory(graphics::Context& context)
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

        if (std::filesystem::exists(imagePath) && std::filesystem::exists(detailsPath)) {
            auto texture = m_context.TextureFromFile(imagePath);

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

            auto const images = json["images"];
            for (auto&& imageJson : images) {
                if (imageJson.contains("path") && imageJson.contains("rect")) {
                    std::filesystem::path relPath;
                    imageJson.at("path").get_to(relPath);
                    auto const absPath = std::filesystem::absolute(rootPath / relPath);
                    ImageDesc desc;
                    desc.m_texture = std::shared_ptr<ITexture>(texture.release());
                    desc.m_path = absPath.string();
                    imageJson.at("rect").get_to(desc.m_sourceRect);
                    m_cachedImages.insert(std::make_pair(desc.m_path, desc));
                }
            }
        }

        return true;
    }

    std::unique_ptr<IImage> ImageFactory::GetImage(std::filesystem::path const& path) {
        auto const cacheIt = m_cachedImages.find(path.string());
        if (std::end(m_cachedImages) != cacheIt) {
            auto const& imageDesc = cacheIt->second;
            IntVec2 const textureDimensions{ imageDesc.m_sourceRect.w(), imageDesc.m_sourceRect.h() };
            return m_context.NewImage(imageDesc.m_texture, imageDesc.m_sourceRect);
        } else {
            if (std::shared_ptr<ITexture> texture = m_context.TextureFromFile(path)) {
                IntVec2 textureDimensions{};
                IntRect sourceRect{ { 0, 0 }, textureDimensions };
                ImageDesc cacheDesc;
                cacheDesc.m_path = path.string();
                cacheDesc.m_sourceRect = sourceRect;
                cacheDesc.m_texture = texture;
                m_cachedImages.insert(std::make_pair(cacheDesc.m_path, cacheDesc));
                return m_context.NewImage(texture, sourceRect);
            }
        }
        return nullptr;
    }
}

