#pragma once

#include "moth_graphics/graphics/texture_factory.h"

#include <moth_ui/iimage_factory.h>
#include <moth_ui/graphics/iimage.h>

#include <memory>
#include <filesystem>

namespace moth_graphics::graphics {
    class MothImageFactory : public moth_ui::IImageFactory {
    public:
        explicit MothImageFactory(graphics::TextureFactory& factoryImpl);
        ~MothImageFactory() override = default;

        std::unique_ptr<moth_ui::IImage> GetImage(std::filesystem::path const& path) override;

    private:
        graphics::TextureFactory& m_factoryImpl;
    };
}

