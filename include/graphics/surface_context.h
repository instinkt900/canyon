#pragma once

#include "graphics/context.h"
#include "graphics/ifont.h"
#include "graphics/iimage.h"
#include "graphics/itexture.h"
#include "utils/rect.h"

namespace graphics {
    class SurfaceContext {
    public:
        virtual ~SurfaceContext() = default;

        virtual Context& GetContext() const = 0;

        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) = 0;
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) = 0;
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) = 0;
        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;
    };
}
