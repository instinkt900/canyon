#include "graphics/moth_ui/moth_image_factory.h"
#include "graphics/moth_ui/moth_image.h"

namespace graphics {
    MothImageFactory::MothImageFactory(Context& context)
        : m_imageFactory(std::make_unique<ImageFactory>(context)) {
    }

    void MothImageFactory::FlushCache() {
        m_imageFactory->FlushCache();
    }

    bool MothImageFactory::LoadTexturePack(std::filesystem::path const& path) {
        return m_imageFactory->LoadTexturePack(path);
    }

    std::unique_ptr<::moth_ui::IImage> MothImageFactory::GetImage(std::filesystem::path const& path) {
        std::shared_ptr<IImage> intlImage = m_imageFactory->GetImage(path);
        return std::make_unique<MothImage>(intlImage);
    }
}
