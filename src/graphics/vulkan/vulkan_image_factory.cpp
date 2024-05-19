#include "canyon.h"
#include "graphics/vulkan/vulkan_image_factory.h"
#include "utils/rect_serialization.h"
#include "graphics/vulkan/vulkan_image.h"

namespace graphics::vulkan {
    ImageFactory::ImageFactory(Context& context, Graphics& graphics)
        : m_context(context) {
    }

    void ImageFactory::FlushCache() {
    }

    bool ImageFactory::LoadTexturePack(std::filesystem::path const& path) {
        auto const rootPath = path.parent_path();
        auto const imagePath = path;
        auto detailsPath = path;
        detailsPath.replace_extension(".json");

        if (std::filesystem::exists(imagePath) && std::filesystem::exists(detailsPath)) {
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

            std::shared_ptr<Texture> packTexture = Texture::FromFile(m_context, imagePath);

            auto const images = json["images"];
            for (auto&& imageJson : images) {
                if (imageJson.contains("path") && imageJson.contains("rect")) {
                    std::filesystem::path relPath;
                    imageJson.at("path").get_to(relPath);
                    auto const absPath = std::filesystem::absolute(rootPath / relPath);
                    ImageDesc desc;
                    desc.m_texture = packTexture;
                    desc.m_path = absPath.string();
                    imageJson.at("rect").get_to(desc.m_sourceRect);
                    m_cachedImages.insert(std::make_pair(desc.m_path, desc));
                }
            }
        }

        return true;
    }

    std::unique_ptr<moth_ui::IImage> ImageFactory::GetImage(std::filesystem::path const& path) {
        auto const cacheIt = m_cachedImages.find(path.string());
        if (std::end(m_cachedImages) != cacheIt) {
            auto const& imageDesc = cacheIt->second;
            IntVec2 const textureDimensions{ imageDesc.m_sourceRect.w(), imageDesc.m_sourceRect.h() };
            return std::make_unique<Image>(imageDesc.m_texture, imageDesc.m_sourceRect);
        } else {
            if (auto texture = Texture::FromFile(m_context, path)) {
                IntVec2 textureDimensions = { texture->GetVkExtent().width, texture->GetVkExtent().height };
                IntRect sourceRect{ { 0, 0 }, textureDimensions };
                ImageDesc cacheDesc;
                cacheDesc.m_path = path.string();
                cacheDesc.m_sourceRect = sourceRect;
                cacheDesc.m_texture = std::move(texture);
                m_cachedImages.insert(std::make_pair(cacheDesc.m_path, cacheDesc));
                return std::make_unique<Image>(cacheDesc.m_texture, sourceRect);
            }
        }
        return nullptr;
    }
}
