#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>

namespace canyon {
    /// @brief Fixed-timestep update loop.
    ///
    /// Drives two callbacks at different rates:
    /// - @c TickFixed() is called at a guaranteed fixed interval (default 60 Hz),
    ///   potentially multiple times per frame to catch up on missed ticks.
    /// - @c Tick() is called once per iteration with the remaining fractional
    ///   milliseconds, suitable for rendering.
    ///
    /// Call @c TickSync() to start the loop. It blocks until @c SetRunning(false)
    /// is called (typically from within an event handler).
    class Ticker {
    public:
        /// @param ticksPerSecond Fixed update rate in Hz. Clamped to 60 if <= 0.
        /// The resulting period is clamped to a minimum of 1ms to avoid a zero interval.
        explicit Ticker(int ticksPerSecond = 60)
            : m_updateTicks(std::chrono::milliseconds(std::max(1, 1000 / (ticksPerSecond > 0 ? ticksPerSecond : 60)))) {
        }
        virtual ~Ticker() {}

        /// @brief Returns the fixed tick interval in milliseconds.
        uint32_t GetFixedTicks() const { return static_cast<uint32_t>(m_updateTicks.count()); }

        /// @brief Start or stop the loop. Call @c SetRunning(false) to exit @c TickSync().
        void SetRunning(bool running) { m_running = running; }

        /// @brief Run the loop synchronously until @c SetRunning(false) is called.
        void TickSync() {
            m_running = true;
            m_lastUpdateTicks = std::chrono::steady_clock::now();
            while (m_running) {
                auto const nowTicks = std::chrono::steady_clock::now();
                auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
                while (deltaTicks >= m_updateTicks) {
                    TickFixed(static_cast<uint32_t>(m_updateTicks.count()));
                    m_lastUpdateTicks += m_updateTicks;
                    deltaTicks -= m_updateTicks;
                }
                Tick(static_cast<uint32_t>(deltaTicks.count()));
                auto const sleepFor = m_updateTicks - deltaTicks;
                if (sleepFor > std::chrono::milliseconds::zero()) {
                    std::this_thread::sleep_for(sleepFor);
                }
            }
        }

    protected:
        /// @brief Called at the fixed update rate.
        /// @param ticks Fixed tick interval in milliseconds (equal to @c GetFixedTicks()).
        virtual void TickFixed(uint32_t ticks) = 0;

        /// @brief Called once per loop iteration with the fractional elapsed time.
        /// @param ticks Milliseconds elapsed since the last fixed tick catchup.
        virtual void Tick(uint32_t ticks) = 0;

    private:
        std::atomic<bool> m_running = false;
        std::chrono::milliseconds m_updateTicks;
        std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
    };
}
