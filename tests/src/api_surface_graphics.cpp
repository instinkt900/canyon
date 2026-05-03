// Pins the method signatures of the core graphics interfaces: IGraphics, Image,
// IFont, and ITarget. Every method pointer assignment fails to compile if a
// method is renamed, removed, or its signature changes.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <memory>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

TEST_CASE("IGraphics method signatures are stable", "[api][graphics][igraphics]") {
    void (IGraphics::*initImgui)(platform::Window const&, bool) = &IGraphics::InitImgui;
    SurfaceContext& (IGraphics::*getSurface)() const            = &IGraphics::GetSurfaceContext;
    bool (IGraphics::*begin)()                                  = &IGraphics::Begin;
    void (IGraphics::*end)()                                  = &IGraphics::End;
    void (IGraphics::*setBlend)(BlendMode)                    = &IGraphics::SetBlendMode;
    void (IGraphics::*setColor)(Color const&)                 = &IGraphics::SetColor;
    void (IGraphics::*clear)()                                = &IGraphics::Clear;
    void (IGraphics::*pushXform)(FloatMat4x4 const&)          = &IGraphics::PushTransform;
    void (IGraphics::*popXform)()                             = &IGraphics::PopTransform;
    void (IGraphics::*drawImg)(Image const&, IntRect const&,
                               IntRect const*)                = &IGraphics::DrawImage;
    void (IGraphics::*drawImgPivot)(Image const&, IntVec2 const&,
                                    FloatVec2 const&)         = &IGraphics::DrawImage;
    void (IGraphics::*drawImgTiled)(Image const&, IntRect const&,
                                    IntRect const*, float)    = &IGraphics::DrawImageTiled;
    void (IGraphics::*drawRect)(FloatRect const&)             = &IGraphics::DrawRectF;
    void (IGraphics::*drawFill)(FloatRect const&)             = &IGraphics::DrawFillRectF;
    void (IGraphics::*drawLine)(FloatVec2 const&,
                                FloatVec2 const&)             = &IGraphics::DrawLineF;
    void (IGraphics::*drawText)(std::string_view, IFont&,
                                IntRect const&,
                                TextHorizAlignment,
                                TextVertAlignment)            = &IGraphics::DrawText;
    void (IGraphics::*setClip)(IntRect const*)                = &IGraphics::SetClip;
    std::unique_ptr<ITarget> (IGraphics::*createTarget)(int,
                                                         int) = &IGraphics::CreateTarget;
    ITarget* (IGraphics::*getTarget)()                        = &IGraphics::GetTarget;
    void (IGraphics::*setTarget)(ITarget*)                    = &IGraphics::SetTarget;
    void (IGraphics::*setLogical)(IntVec2 const&)             = &IGraphics::SetLogicalSize;

    (void)initImgui; (void)getSurface; (void)begin; (void)end;
    (void)setBlend; (void)setColor; (void)clear;
    (void)pushXform; (void)popXform;
    (void)drawImg; (void)drawImgPivot; (void)drawImgTiled;
    (void)drawRect; (void)drawFill; (void)drawLine; (void)drawText;
    (void)setClip; (void)createTarget; (void)getTarget; (void)setTarget;
    (void)setLogical;
    SUCCEED();
}

TEST_CASE("Image method signatures are stable", "[api][graphics][image]") {
    int (Image::*getW)() const                                            = &Image::GetWidth;
    int (Image::*getH)() const                                            = &Image::GetHeight;
    std::shared_ptr<ITexture> const& (Image::*getTex)() const             = &Image::GetTexture;
    IntRect const& (Image::*getSrc)() const                               = &Image::GetSourceRect;
    void (Image::*imguiSrc)(IntVec2 const&) const                          = &Image::DrawImGui;
    void (Image::*imguiRaw)(IntVec2 const&, FloatVec2 const&,
                            FloatVec2 const&) const                         = &Image::DrawImGui;
    void (Image::*savePng)(std::filesystem::path const&)                   = &Image::SaveToPNG;
    (void)getW; (void)getH; (void)getTex; (void)getSrc; (void)imguiSrc; (void)imguiRaw; (void)savePng;
    SUCCEED();
}

TEST_CASE("IFont method signatures are stable", "[api][graphics][ifont]") {
    IntVec2 (IFont::*measure)(std::string_view) const = &IFont::Measure;
    (void)measure;
    SUCCEED();
}

TEST_CASE("ITarget method signatures are stable", "[api][graphics][itarget]") {
    Image (ITarget::*getImg)() const = &ITarget::GetImage;
    (void)getImg;
    SUCCEED();
}
