#pragma once

#include <chrono>

namespace canyon {
    class Ticker {
    public:
        Ticker(int ticksPerSecond = 60) {
            m_updateTicks = std::chrono::milliseconds(1000 / ticksPerSecond);
        }
        virtual ~Ticker() {}

        void SetRunning(bool running) { m_running = running; }

        void TickSync() {
            m_running = true;
            m_lastUpdateTicks = std::chrono::steady_clock::now();
            while (m_running) {
                auto const nowTicks = std::chrono::steady_clock::now();
                auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
                while (deltaTicks > m_updateTicks) {
                    Tick(static_cast<uint32_t>(m_updateTicks.count()));
                    m_lastUpdateTicks += m_updateTicks;
                    deltaTicks -= m_updateTicks;
                }
            }
        }

    protected:
        virtual void Tick(uint32_t ticks) = 0;

    private:
        bool m_running = false;
        std::chrono::milliseconds m_updateTicks;
        std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
    };
}
