#include "canyon.h"
#include "graphics/sdl/sdl_image_factory.h"
#include "graphics/sdl/sdl_image.h"
#include "utils/rect_serialization.h"

namespace graphics::sdl {
    ImageFactory::ImageFactory(SDL_Renderer& renderer)
        : m_renderer(renderer) {
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
            auto const texture = CreateTextureRef(&m_renderer, imagePath);

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
                    desc.m_texture = texture;
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
            return std::make_unique<Image>(imageDesc.m_texture, textureDimensions, imageDesc.m_sourceRect);
        } else {
            if (auto texture = CreateTextureRef(&m_renderer, path)) {
                IntVec2 textureDimensions{};
                SDL_QueryTexture(texture->GetImpl(), NULL, NULL, &textureDimensions.x, &textureDimensions.y);
                IntRect sourceRect{ { 0, 0 }, textureDimensions };
                ImageDesc cacheDesc;
                cacheDesc.m_path = path.string();
                cacheDesc.m_sourceRect = sourceRect;
                cacheDesc.m_texture = texture;
                m_cachedImages.insert(std::make_pair(cacheDesc.m_path, cacheDesc));
                return std::make_unique<Image>(texture, textureDimensions, sourceRect);
            }
        }
        return nullptr;
    }
}
