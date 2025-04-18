#pragma once

namespace canyon::graphics {
    class IImage {
    public:
        virtual ~IImage() = default;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
    };
}
