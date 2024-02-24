#pragma once

#include "graphics/iimage.h"
#include "graphics/blend_mode.h"
#include "graphics/color.h"
#include "moth_ui/utils/rect.h"
#include "moth_ui/text_alignment.h"

#include <filesystem>

namespace backend {
    class IGraphicsContext {
    public:
	virtual ~IGraphicsContext() {}
        virtual void SetBlendMode(moth_ui::BlendMode mode) = 0;
        //virtual void SetBlendMode(std::shared_ptr<moth_ui::IImage> target, EBlendMode mode) = 0;
        //virtual void SetColorMod(std::shared_ptr<moth_ui::IImage> target, moth_ui::Color const& color) = 0;
        virtual void SetColor(moth_ui::Color const& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawImage(moth_ui::IImage& image, moth_ui::IntRect const* sourceRect, moth_ui::IntRect const* destRect) = 0;
        virtual void DrawToPNG(std::filesystem::path const& path) = 0;
        virtual void DrawRectF(moth_ui::FloatRect const& rect) = 0;
        virtual void DrawFillRectF(moth_ui::FloatRect const& rect) = 0;
        virtual void DrawLineF(moth_ui::FloatVec2 const& p0, moth_ui::FloatVec2 const& p1) = 0;
        virtual void DrawText(std::string const& text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) = 0;

        virtual std::unique_ptr<moth_ui::ITarget> CreateTarget(int width, int height) = 0;
        virtual moth_ui::ITarget* GetTarget() = 0;
        virtual void SetTarget(moth_ui::ITarget* target) = 0;

        virtual void SetLogicalSize(moth_ui::IntVec2 const& logicalSize) = 0;
    };
}
