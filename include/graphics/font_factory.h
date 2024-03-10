#pragma once

#include "ifont.h"

namespace graphics {
    class FontFactory {
    public:
        virtual ~FontFactory() = default;

        virtual void AddFont(char const* name, std::filesystem::path const& path);
        virtual void RemoveFont(char const* name);
        virtual void LoadProject(std::filesystem::path const& path);
        virtual void SaveProject(std::filesystem::path const& path);
        virtual std::filesystem::path GetCurrentProjectPath() const { return m_currentProjectPath; }
        virtual void ClearFonts();
        virtual std::shared_ptr<IFont> GetDefaultFont(int size);
        virtual std::vector<std::string> GetFontNameList() const;
        virtual std::shared_ptr<IFont> GetFont(char const* name, int size) = 0;
        virtual std::filesystem::path GetFontPath(char const* name) const;

    protected:
        std::map<std::string, std::filesystem::path> m_fontPaths;
        std::filesystem::path m_currentProjectPath;
    };
}
