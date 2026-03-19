#pragma once

#include "canyon/graphics/asset_context.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itexture.h"
#include "canyon/utils/rect.h"

#include <filesystem>
#include <memory>

namespace canyon::graphics::vulkan {
    class SurfaceContext;

    class AssetContext : public graphics::AssetContext {
    public:
        explicit AssetContext(SurfaceContext& context);
        ~AssetContext() override = default;

        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture) override;
        std::unique_ptr<IImage> NewImage(std::shared_ptr<ITexture> texture, IntRect const& sourceRect) override;
        std::unique_ptr<IFont> FontFromFile(std::filesystem::path const& path, uint32_t size) override;
        std::unique_ptr<ITexture> TextureFromFile(std::filesystem::path const& path) override;
        std::unique_ptr<IImage> ImageFromFile(std::filesystem::path const& path) override;

    private:
        SurfaceContext& m_context;
    };
}
