#include "moth_graphics/graphics/sprite.h"
#include "moth_graphics/graphics/spritesheet.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/vector.h"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

using namespace moth_graphics;
using namespace moth_graphics::graphics;

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

namespace {
    SpriteSheet::FrameEntry MakeFrame(int x, int y, int w, int h, int px = 0, int py = 0) {
        return { MakeRect(x, y, w, h), IntVec2{ px, py } };
    }

    // Build a SpriteSheet with N atlas frames (8x8 each) and the given clips.
    // Image is empty (no GPU texture) — sufficient for tests that don't render.
    std::shared_ptr<SpriteSheet> MakeSheet(
        int numFrames,
        std::vector<SpriteSheet::ClipEntry> clips = {}) {
        std::vector<SpriteSheet::FrameEntry> frames;
        frames.reserve(static_cast<size_t>(numFrames));
        for (int i = 0; i < numFrames; ++i) {
            frames.push_back(MakeFrame(i * 8, 0, 8, 8, i, i));
        }
        return std::make_shared<SpriteSheet>(Image{}, std::move(frames), std::move(clips));
    }

    // Build a two-frame clip entry with equal step durations.
    SpriteSheet::ClipEntry MakeClipEntry(
        std::string name,
        std::vector<SpriteSheet::ClipFrame> steps,
        SpriteSheet::LoopType loop = SpriteSheet::LoopType::Stop) {
        return { std::move(name), { std::move(steps), loop } };
    }
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST_CASE("Sprite::Create returns nullptr for null sheet", "[sprite][create]") {
    REQUIRE(Sprite::Create(nullptr) == nullptr);
}

TEST_CASE("Sprite::Create returns nullptr for empty sheet", "[sprite][create]") {
    auto sheet = std::make_shared<SpriteSheet>(Image{},
                                               std::vector<SpriteSheet::FrameEntry>{},
                                               std::vector<SpriteSheet::ClipEntry>{});
    REQUIRE(Sprite::Create(sheet) == nullptr);
}

TEST_CASE("Sprite::Create succeeds for a valid sheet", "[sprite][create]") {
    auto sheet = MakeSheet(2);
    REQUIRE(Sprite::Create(sheet) != nullptr);
}

// ---------------------------------------------------------------------------
// Initial state
// ---------------------------------------------------------------------------

TEST_CASE("Sprite starts not playing with no clip", "[sprite][initial]") {
    auto sprite = Sprite::Create(MakeSheet(2));
    REQUIRE_FALSE(sprite->IsPlaying());
    REQUIRE(sprite->GetCurrentClipName().empty());
}

TEST_CASE("Sprite starts on atlas frame 0", "[sprite][initial]") {
    auto sprite = Sprite::Create(MakeSheet(3));
    // No clip active — GetCurrentFrame() returns the raw atlas index.
    REQUIRE(sprite->GetCurrentFrame() == 0);
}

// ---------------------------------------------------------------------------
// SetClip
// ---------------------------------------------------------------------------

TEST_CASE("SetClip selects a valid clip without changing playing state", "[sprite][set_clip]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 }, { 1, 100 } })
    });
    auto sprite = Sprite::Create(sheet);

    // Initially not playing; SetClip should leave it not playing.
    sprite->SetClip("run");
    REQUIRE(sprite->GetCurrentClipName() == "run");
    REQUIRE_FALSE(sprite->IsPlaying());
}

TEST_CASE("SetClip on unknown name clears clip and stops playing", "[sprite][set_clip]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");
    sprite->SetPlaying(true);
    REQUIRE(sprite->IsPlaying());

    sprite->SetClip("nonexistent");
    REQUIRE(sprite->GetCurrentClipName().empty());
    REQUIRE_FALSE(sprite->IsPlaying());
}

TEST_CASE("SetClip with empty string clears clip and stops playing", "[sprite][set_clip]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");
    sprite->SetPlaying(true);

    sprite->SetClip("");
    REQUIRE(sprite->GetCurrentClipName().empty());
    REQUIRE_FALSE(sprite->IsPlaying());
}

// ---------------------------------------------------------------------------
// SetPlaying
// ---------------------------------------------------------------------------

TEST_CASE("SetPlaying has no effect without an active clip", "[sprite][set_playing]") {
    auto sprite = Sprite::Create(MakeSheet(2));
    sprite->SetPlaying(true);
    REQUIRE_FALSE(sprite->IsPlaying());
}

TEST_CASE("SetPlaying resumes after SetClip selects a valid clip", "[sprite][set_playing]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 }, { 1, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");
    sprite->SetPlaying(true);
    REQUIRE(sprite->IsPlaying());
    sprite->SetPlaying(false);
    REQUIRE_FALSE(sprite->IsPlaying());
}

// ---------------------------------------------------------------------------
// Update — LoopType::Stop
// ---------------------------------------------------------------------------

TEST_CASE("Update does not advance frames when not playing", "[sprite][update]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("idle", { { 0, 100 }, { 1, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("idle");
    // Not playing — Update should be a no-op.
    sprite->Update(200);
    REQUIRE(sprite->GetCurrentFrame() == 0);
}

TEST_CASE("Update advances to next clip step when duration elapses", "[sprite][update]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 }, { 1, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");
    sprite->SetPlaying(true);

    sprite->Update(100);  // exactly one step duration
    // Should now be on clip step 1, which maps to atlas frame 1.
    REQUIRE(sprite->GetCurrentFrame() == 1);
    REQUIRE(sprite->IsPlaying());
}

TEST_CASE("LoopType::Stop freezes on last frame and stops playing", "[sprite][loop][stop]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("anim", { { 0, 100 }, { 1, 100 } }, SpriteSheet::LoopType::Stop)
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("anim");
    sprite->SetPlaying(true);

    sprite->Update(250);  // well past both steps
    REQUIRE_FALSE(sprite->IsPlaying());
    REQUIRE(sprite->GetCurrentFrame() == 1);  // frozen on last atlas frame
}

TEST_CASE("LoopType::Reset rewinds to frame 0 and stops playing", "[sprite][loop][reset]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("anim", { { 0, 100 }, { 1, 100 } }, SpriteSheet::LoopType::Reset)
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("anim");
    sprite->SetPlaying(true);

    sprite->Update(250);
    REQUIRE_FALSE(sprite->IsPlaying());
    REQUIRE(sprite->GetCurrentFrame() == 0);  // reset to first atlas frame
}

TEST_CASE("LoopType::Loop wraps back to first step and keeps playing", "[sprite][loop][loop]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("anim", { { 0, 100 }, { 1, 100 } }, SpriteSheet::LoopType::Loop)
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("anim");
    sprite->SetPlaying(true);

    sprite->Update(250);  // past end; wraps: 250-100-100=50ms into step 0
    REQUIRE(sprite->IsPlaying());
    REQUIRE(sprite->GetCurrentFrame() == 0);
}

// ---------------------------------------------------------------------------
// SetFrame
// ---------------------------------------------------------------------------

TEST_CASE("SetFrame clamps to valid range", "[sprite][set_frame]") {
    auto sprite = Sprite::Create(MakeSheet(3));
    sprite->SetFrame(-1);
    REQUIRE(sprite->GetCurrentFrame() == 0);
    sprite->SetFrame(100);
    REQUIRE(sprite->GetCurrentFrame() == 2);
}

TEST_CASE("SetFrame clears clip and stops playing", "[sprite][set_frame]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 0, 100 } })
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");
    sprite->SetPlaying(true);

    sprite->SetFrame(1);
    REQUIRE_FALSE(sprite->IsPlaying());
    REQUIRE(sprite->GetCurrentClipName().empty());
    REQUIRE(sprite->GetCurrentFrame() == 1);
}

// ---------------------------------------------------------------------------
// GetCurrentFrameRect / GetCurrentFramePivot
// ---------------------------------------------------------------------------

TEST_CASE("GetCurrentFrameRect returns the atlas rect for the active frame", "[sprite][frame_rect]") {
    auto sheet = MakeSheet(2, {
        MakeClipEntry("run", { { 1, 100 } })  // clip step maps to atlas frame 1
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");

    auto const rect = sprite->GetCurrentFrameRect();
    // Atlas frame 1 starts at x=8, width=8 (from MakeSheet helper).
    REQUIRE(rect.x() == 8);
    REQUIRE(rect.w() == 8);
    REQUIRE(rect.h() == 8);
}

TEST_CASE("GetCurrentFramePivot returns the per-frame pivot", "[sprite][frame_pivot]") {
    // MakeSheet sets pivot = {frameIndex, frameIndex} for each frame.
    auto sheet = MakeSheet(3, {
        MakeClipEntry("run", { { 2, 100 } })  // atlas frame 2 -> pivot {2,2}
    });
    auto sprite = Sprite::Create(sheet);
    sprite->SetClip("run");

    auto const pivot = sprite->GetCurrentFramePivot();
    REQUIRE(pivot.x == 2);
    REQUIRE(pivot.y == 2);
}
