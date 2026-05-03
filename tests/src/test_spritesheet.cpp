#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/vector.h"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

namespace {
    // Empty Image — no GPU resources required for these tests.
    Image MakeDummyImage() {
        return Image{};
    }

    SpriteSheet::FrameEntry MakeFrame(int x, int y, int w, int h, int px = 0, int py = 0) {
        return { MakeRect(x, y, w, h), IntVec2{ px, py } };
    }

    SpriteSheet::ClipEntry MakeClip(std::string name,
                                    std::vector<SpriteSheet::ClipFrame> frames,
                                    SpriteSheet::LoopType loop = SpriteSheet::LoopType::Stop) {
        return { std::move(name), { std::move(frames), loop } };
    }
}

TEST_CASE("SpriteSheet reports correct frame count", "[spritesheet]") {
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 16, 16), MakeFrame(16, 0, 16, 16) }, {});
    REQUIRE(sheet.GetFrameCount() == 2);
}

TEST_CASE("SpriteSheet GetFrameDesc returns correct rect and pivot", "[spritesheet]") {
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(4, 8, 32, 16, 5, 3) }, {});

    SpriteSheet::FrameEntry entry{};
    REQUIRE(sheet.GetFrameDesc(0, entry));
    REQUIRE(entry.rect.x() == 4);
    REQUIRE(entry.rect.y() == 8);
    REQUIRE(entry.rect.w() == 32);
    REQUIRE(entry.rect.h() == 16);
    REQUIRE(entry.pivot.x == 5);
    REQUIRE(entry.pivot.y == 3);
}

TEST_CASE("SpriteSheet GetFrameDesc returns false for out-of-range index", "[spritesheet]") {
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 8, 8) }, {});

    SpriteSheet::FrameEntry entry{};
    REQUIRE_FALSE(sheet.GetFrameDesc(-1, entry));
    REQUIRE_FALSE(sheet.GetFrameDesc(1, entry));
}

TEST_CASE("SpriteSheet reports correct clip count", "[spritesheet]") {
    std::vector<SpriteSheet::ClipEntry> clips{
        MakeClip("run",  { { 0, 100 }, { 1, 100 } }),
        MakeClip("idle", { { 0, 200 } }),
    };
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 8, 8), MakeFrame(8, 0, 8, 8) }, clips);
    REQUIRE(sheet.GetClipCount() == 2);
}

TEST_CASE("SpriteSheet GetClipName returns names in order", "[spritesheet]") {
    std::vector<SpriteSheet::ClipEntry> clips{
        MakeClip("alpha", { { 0, 100 } }),
        MakeClip("beta",  { { 0, 100 } }),
    };
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 8, 8) }, clips);
    REQUIRE(sheet.GetClipName(0) == "alpha");
    REQUIRE(sheet.GetClipName(1) == "beta");
    REQUIRE(sheet.GetClipName(2).empty());
}

TEST_CASE("SpriteSheet GetClipDesc returns correct clip data", "[spritesheet]") {
    std::vector<SpriteSheet::ClipFrame> steps{ { 0, 80 }, { 1, 120 } };
    std::vector<SpriteSheet::ClipEntry> clips{
        MakeClip("walk", steps, SpriteSheet::LoopType::Loop),
    };
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 8, 8), MakeFrame(8, 0, 8, 8) }, clips);

    SpriteSheet::ClipDesc desc{};
    REQUIRE(sheet.GetClipDesc("walk", desc));
    REQUIRE(desc.loop == SpriteSheet::LoopType::Loop);
    REQUIRE(desc.frames.size() == 2);
    REQUIRE(desc.frames[0].frameIndex == 0);
    REQUIRE(desc.frames[0].durationMs == 80);
    REQUIRE(desc.frames[1].frameIndex == 1);
    REQUIRE(desc.frames[1].durationMs == 120);
}

TEST_CASE("SpriteSheet GetClipDesc returns false for unknown clip name", "[spritesheet]") {
    SpriteSheet sheet(MakeDummyImage(), { MakeFrame(0, 0, 8, 8) }, {});
    SpriteSheet::ClipDesc desc{};
    REQUIRE_FALSE(sheet.GetClipDesc("missing", desc));
}

TEST_CASE("SpriteSheet GetImage returns the image passed at construction", "[spritesheet]") {
    SpriteSheet sheet(Image{}, { MakeFrame(0, 0, 8, 8) }, {});
    // Default-constructed Image is empty; matching here just verifies the round-trip.
    REQUIRE(static_cast<bool>(sheet.GetImage()) == false);
}
