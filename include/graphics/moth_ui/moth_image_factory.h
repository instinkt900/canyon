#pragma once

#include "graphics/image_factory.h"

#include <moth_ui/iimage_factory.h>

#include <memory>
#include <filesystem>

namespace graphics {
    class MothImageFactory : public moth_ui::IImageFactory {
    public:
        MothImageFactory(std::shared_ptr<graphics::ImageFactory> factoryImpl);
        virtual ~MothImageFactory() = default;

        void FlushCache() override;
        bool LoadTexturePack(std::filesystem::path const& path) override;
        std::unique_ptr<moth_ui::IImage> GetImage(std::filesystem::path const& path) override;

    private:
        std::shared_ptr<ImageFactory> m_factoryImpl;
    };
}

