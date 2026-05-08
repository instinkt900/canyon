// Pins the signatures of the moth_ui bridge adapters: MothImage, MothRenderer,
// MothImageFactory, and MothFontFactory.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

using namespace moth_graphics::graphics;

TEST_CASE("MothImage method signatures are stable", "[api][bridge][moth_image]") {
    int  (MothImage::*getW)() const                                  = &MothImage::GetWidth;
    int  (MothImage::*getH)() const                                  = &MothImage::GetHeight;
    moth_ui::IntVec2 (MothImage::*getDims)() const                   = &MothImage::GetDimensions;
    moth_graphics::graphics::Image const&
         (MothImage::*getImg)() const                                = &MothImage::GetImage;
    (void)getW; (void)getH; (void)getDims; (void)getImg;
    SUCCEED();
}

TEST_CASE("MothRenderer method signatures are stable", "[api][bridge][moth_renderer]") {
    void (MothRenderer::*pushBlend)(moth_ui::BlendMode)              = &MothRenderer::PushBlendMode;
    void (MothRenderer::*popBlend)()                                 = &MothRenderer::PopBlendMode;
    void (MothRenderer::*pushColor)(moth_ui::Color const&)           = &MothRenderer::PushColor;
    void (MothRenderer::*popColor)()                                 = &MothRenderer::PopColor;
    void (MothRenderer::*pushXform)(moth_ui::FloatMat4x4 const&)    = &MothRenderer::PushTransform;
    void (MothRenderer::*popXform)()                                 = &MothRenderer::PopTransform;
    void (MothRenderer::*pushClip)(moth_ui::IntRect const&)          = &MothRenderer::PushClip;
    void (MothRenderer::*popClip)()                                  = &MothRenderer::PopClip;
    void (MothRenderer::*renderRect)(moth_ui::IntRect const&)        = &MothRenderer::RenderRect;
    void (MothRenderer::*renderFill)(moth_ui::IntRect const&)        = &MothRenderer::RenderFilledRect;
    void (MothRenderer::*renderImg)(moth_ui::IImage const&,
                                    moth_ui::IntRect const&,
                                    moth_ui::IntRect const&,
                                    moth_ui::ImageScaleType,
                                    float)                           = &MothRenderer::RenderImage;
    void (MothRenderer::*renderText)(std::string_view,
                                     moth_ui::IFont&,
                                     moth_ui::TextHorizAlignment,
                                     moth_ui::TextVertAlignment,
                                     moth_ui::IntRect const&)        = &MothRenderer::RenderText;
    void (MothRenderer::*setLogical)(moth_ui::IntVec2 const&)        = &MothRenderer::SetRendererLogicalSize;

    (void)pushBlend; (void)popBlend; (void)pushColor; (void)popColor;
    (void)pushXform; (void)popXform; (void)pushClip; (void)popClip;
    (void)renderRect; (void)renderFill; (void)renderImg; (void)renderText;
    (void)setLogical;
    SUCCEED();
}

TEST_CASE("MothImageFactory method signatures are stable", "[api][bridge][moth_image_factory]") {
    std::unique_ptr<moth_ui::IImage>
         (MothImageFactory::*getImg)(std::filesystem::path const&)              = &MothImageFactory::GetImage;
    (void)getImg;
    SUCCEED();
}

TEST_CASE("MothFontFactory method signatures are stable", "[api][bridge][moth_font_factory]") {
    void (MothFontFactory::*clear)()                                            = &MothFontFactory::ClearFonts;
    std::shared_ptr<moth_ui::IFont>
         (MothFontFactory::*getFont)(std::string const&, int)                   = &MothFontFactory::GetFont;
    (void)clear; (void)getFont;
    SUCCEED();
}
