#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_image_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"

namespace moth_graphics::graphics {
    MothImageFactory::MothImageFactory(graphics::TextureFactory& factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    std::unique_ptr<::moth_ui::IImage> MothImageFactory::GetImage(std::filesystem::path const& path) {
        auto texture = m_factoryImpl.GetTexture(path);
        if (!texture) {
            return nullptr;
        }
        auto const sourceRect = m_factoryImpl.GetTextureRect(path);
        return std::make_unique<MothImage>(Image{ texture, sourceRect });
    }
}
