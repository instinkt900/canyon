#pragma once

#include "graphics/igraphics.h"
#include "graphics/iimage.h"
#include "graphics/itexture.h"
#include "utils/rect.h"

namespace graphics {
    class Context {
    public:
        virtual ~Context() = default;

        virtual std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) = 0;
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) = 0;
        virtual std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) = 0;
        virtual std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, int size) = 0;
    };
}

