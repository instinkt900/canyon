#pragma once

namespace canyon::graphics {
    class ITexture {
    public:
        virtual ~ITexture() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
    };
}
