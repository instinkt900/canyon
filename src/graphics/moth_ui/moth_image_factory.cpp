#include "common.h"
#include "moth_graphics/graphics/moth_ui/moth_image_factory.h"
#include "moth_graphics/graphics/moth_ui/moth_image.h"

namespace moth_graphics::graphics {
    MothImageFactory::MothImageFactory(graphics::ImageFactory& factoryImpl)
        : m_factoryImpl(factoryImpl) {
    }

    void MothImageFactory::FlushCache() {
        m_factoryImpl.FlushCache();
    }

    bool MothImageFactory::LoadTexturePack(std::filesystem::path const& path) {
        return m_factoryImpl.LoadTexturePack(path);
    }

    std::unique_ptr<::moth_ui::IImage> MothImageFactory::GetImage(std::filesystem::path const& path) {
        std::shared_ptr<IImage> intlImage = m_factoryImpl.GetImage(path);
        return std::make_unique<MothImage>(intlImage);
    }
}
