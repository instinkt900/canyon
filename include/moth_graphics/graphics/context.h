#pragma once

namespace moth_graphics::graphics {
    class Context {
    public:
        virtual ~Context() = default;

        virtual bool Startup() = 0;
        virtual void Shutdown() = 0;
    };
}
