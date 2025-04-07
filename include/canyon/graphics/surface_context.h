#pragma once

#include "canyon/graphics/context.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itexture.h"
#include "canyon/utils/rect.h"

#include <memory>
#include <filesystem>
#include <cstdint>

namespace graphics {
    class SurfaceContext {
    public:
        virtual ~SurfaceContext() = default;

        virtual Context& GetContext() const = 0;

        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) = 0;
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) = 0;
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) = 0;
        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;
        virtual std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) = 0;
    };
}
