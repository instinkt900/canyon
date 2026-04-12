// Pins the method signatures of SpriteSheet and Sprite.

#include "moth_graphics/moth_graphics.h"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <string_view>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

TEST_CASE("SpriteSheet method signatures are stable", "[api][sprite][spritesheet]") {
    // Nested type field layout
    static_assert(std::is_same_v<decltype(SpriteSheet::FrameEntry::rect),  IntRect>);
    static_assert(std::is_same_v<decltype(SpriteSheet::FrameEntry::pivot), IntVec2>);
    static_assert(std::is_same_v<decltype(SpriteSheet::ClipFrame::frameIndex), int>);
    static_assert(std::is_same_v<decltype(SpriteSheet::ClipFrame::durationMs), int>);
    static_assert(std::is_same_v<decltype(SpriteSheet::ClipDesc::frames),
                                 std::vector<SpriteSheet::ClipFrame>>);
    static_assert(std::is_same_v<decltype(SpriteSheet::ClipDesc::loop),
                                 SpriteSheet::LoopType>);

    // Method signatures
    std::shared_ptr<IImage> (SpriteSheet::*getImg)() const                          = &SpriteSheet::GetImage;
    int  (SpriteSheet::*getFrameCount)() const                                       = &SpriteSheet::GetFrameCount;
    bool (SpriteSheet::*getFrameDesc)(int, SpriteSheet::FrameEntry&) const           = &SpriteSheet::GetFrameDesc;
    int  (SpriteSheet::*getClipCount)() const                                        = &SpriteSheet::GetClipCount;
    std::string_view (SpriteSheet::*getClipName)(int) const                          = &SpriteSheet::GetClipName;
    bool (SpriteSheet::*getClipDesc)(std::string_view,
                                     SpriteSheet::ClipDesc&) const                  = &SpriteSheet::GetClipDesc;

    (void)getImg; (void)getFrameCount; (void)getFrameDesc;
    (void)getClipCount; (void)getClipName; (void)getClipDesc;
    SUCCEED();
}

TEST_CASE("Sprite method signatures are stable", "[api][sprite][sprite]") {
    SpriteSheet const& (Sprite::*getSheet)() const       = &Sprite::GetSpriteSheet;
    void (Sprite::*setClip)(std::string_view)             = &Sprite::SetClip;
    void (Sprite::*setPlaying)(bool)                      = &Sprite::SetPlaying;
    void (Sprite::*update)(uint32_t)                      = &Sprite::Update;
    bool (Sprite::*isPlaying)() const                     = &Sprite::IsPlaying;
    std::string_view (Sprite::*getClipName)() const       = &Sprite::GetCurrentClipName;
    int  (Sprite::*getCurrentFrame)() const               = &Sprite::GetCurrentFrame;
    void (Sprite::*setFrame)(int)                         = &Sprite::SetFrame;
    IntRect (Sprite::*getFrameRect)() const               = &Sprite::GetCurrentFrameRect;
    IntVec2 (Sprite::*getFramePivot)() const              = &Sprite::GetCurrentFramePivot;
    int  (Sprite::*getWidth)() const                      = &Sprite::GetWidth;
    int  (Sprite::*getHeight)() const                     = &Sprite::GetHeight;
    IImage* (Sprite::*getImage)() const                   = &Sprite::GetImage;

    (void)getSheet; (void)setClip; (void)setPlaying; (void)update;
    (void)isPlaying; (void)getClipName; (void)getCurrentFrame; (void)setFrame;
    (void)getFrameRect; (void)getFramePivot; (void)getWidth; (void)getHeight;
    (void)getImage;
    SUCCEED();
}
