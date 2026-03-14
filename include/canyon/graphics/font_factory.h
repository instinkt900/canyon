#pragma once

#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/ifont.h"

#include <map>
#include <string>
#include <memory>

namespace canyon::graphics {
    /// @brief Cached font loader.
    ///
    /// Loads fonts from disk on first request and caches them by name and size.
    /// Fonts are shared — multiple callers requesting the same name/size receive
    /// the same @c IFont instance.
    class FontFactory {
    public:
        /// @param context The surface context used to create GPU resources.
        FontFactory(SurfaceContext& context);
        virtual ~FontFactory() = default;

        /// @brief Release all cached fonts.
        void ClearFonts();

        /// @brief Load or retrieve a cached font by file path and point size.
        /// @param name Path to the font file (TTF/OTF).
        /// @param size Point size to load.
        /// @returns Shared font handle, or @c nullptr on failure.
        std::shared_ptr<IFont> GetFont(std::string const& name, int size);

    private:
        SurfaceContext& m_context;
        std::map<std::string, std::map<int, std::shared_ptr<IFont>>> m_fonts;
    };
}
