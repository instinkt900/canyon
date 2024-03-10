#pragma once

#include "utils/rect.h"
#include "graphics/iimage.h"
#include "graphics/ifont.h"
#include "graphics/color.h"
#include "graphics/blend_mode.h"
#include "graphics/text_alignment.h"
#include "graphics/image_scale_type.h"

namespace graphics {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void PushBlendMode(BlendMode mode) = 0;
        virtual void PopBlendMode() = 0;
        virtual void PushColor(Color const& color) = 0;
        virtual void PopColor() = 0;

        virtual void PushClip(IntRect const& rect) = 0;
        virtual void PopClip() = 0;

        virtual void RenderRect(IntRect const& rect) = 0;
        virtual void RenderFilledRect(IntRect const& rect) = 0;
        virtual void RenderImage(IImage& image, IntRect const& sourceRect, IntRect const& destRect, ImageScaleType scaleType, float scale) = 0;
        virtual void RenderText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) = 0;
    };
}
