#pragma once

#include "graphics/igraphics.h"

namespace graphics {
    class FontFactory {
    public:
        FontFactory(SurfaceContext& context);
        virtual ~FontFactory() = default;

        void AddFont(char const* name, std::filesystem::path const& path);
        void RemoveFont(char const* name);
        void LoadProject(std::filesystem::path const& path);
        void SaveProject(std::filesystem::path const& path);
        std::filesystem::path GetCurrentProjectPath() const { return m_currentProjectPath; }
        void ClearFonts();
        std::shared_ptr<IFont> GetDefaultFont(int size);
        std::vector<std::string> GetFontNameList() const;
        std::filesystem::path GetFontPath(char const* name) const;
        std::shared_ptr<IFont> GetFont(char const* name, int size);

    private:
        SurfaceContext& m_context;
        std::filesystem::path m_currentProjectPath;
        std::map<std::string, std::filesystem::path> m_fontPaths;
        std::map<std::string, std::map<int, std::shared_ptr<graphics::IFont>>> m_fonts;
    };
}
