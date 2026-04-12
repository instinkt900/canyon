#include "moth_graphics/utils/ticker.h"

#include <catch2/catch_all.hpp>
#include <atomic>
#include <cstdint>
#include <thread>

using namespace moth_graphics;

// ---------------------------------------------------------------------------
// Concrete subclass for testing — just counts calls.
// ---------------------------------------------------------------------------

namespace {
    class TestTicker : public Ticker {
    public:
        explicit TestTicker(int rate = 60) : Ticker(rate) {}

        std::atomic<int> fixedCount{ 0 };
        std::atomic<int> tickCount{ 0 };

    protected:
        void TickFixed(uint32_t /*ticks*/) override {
            ++fixedCount;
            // Stop after 3 fixed ticks so the test doesn't run forever.
            if (fixedCount >= 3) {
                SetRunning(false);
            }
        }

        void Tick(uint32_t /*ticks*/) override {
            ++tickCount;
        }
    };
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

TEST_CASE("Ticker default rate is 60 Hz (16 ms period)", "[ticker][construction]") {
    Ticker* t = new TestTicker(60);
    // 1000 / 60 = 16 ms
    REQUIRE(t->GetFixedTicks() == 16);
    delete t;
}

TEST_CASE("Ticker clamped to 60 Hz for zero or negative rate", "[ticker][construction]") {
    TestTicker t0(0);
    REQUIRE(t0.GetFixedTicks() == 16);

    TestTicker tneg(-10);
    REQUIRE(tneg.GetFixedTicks() == 16);
}

TEST_CASE("Ticker period is correct for 30 Hz", "[ticker][construction]") {
    TestTicker t(30);
    // 1000 / 30 = 33 ms
    REQUIRE(t.GetFixedTicks() == 33);
}

// ---------------------------------------------------------------------------
// TickSync
// ---------------------------------------------------------------------------

TEST_CASE("TickSync calls TickFixed and Tick at least once", "[ticker][ticksync]") {
    TestTicker t(200);  // fast rate so test completes quickly
    // Run on a background thread so we don't block.
    std::thread runner([&] { t.TickSync(); });
    runner.join();

    REQUIRE(t.fixedCount >= 1);
    REQUIRE(t.tickCount >= 1);
}

TEST_CASE("SetRunning(false) stops TickSync", "[ticker][ticksync]") {
    // TestTicker stops itself after 3 fixed ticks.
    TestTicker t(1000);  // 1 ms period — exits fast
    std::thread runner([&] { t.TickSync(); });
    runner.join();

    REQUIRE(t.fixedCount == 3);
}
