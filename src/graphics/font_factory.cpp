#include "canyon.h"
#include "graphics/font_factory.h"

namespace graphics {
    void FontFactory::AddFont(char const* name, std::filesystem::path const& path) {
        m_fontPaths[name] = path;
    }

    void FontFactory::RemoveFont(char const* name) {
        m_fontPaths.erase(name);
    }

    void FontFactory::LoadProject(std::filesystem::path const& path) {
        ClearFonts();

        std::ifstream ifile(path);
        if (!ifile.is_open()) {
            return;
        }

        nlohmann::json json;
        ifile >> json;

        std::filesystem::path rootPath = path.parent_path();
        std::map<std::string, std::filesystem::path> relativeList;
        if (json.contains("fonts")) {
            json.at("fonts").get_to(relativeList);

            for (auto& [name, relPath] : relativeList) {
                AddFont(name.c_str(), std::filesystem::absolute(rootPath / relPath));
            }
        }

        m_currentProjectPath = path;
    }

    void FontFactory::SaveProject(std::filesystem::path const& path) {
        std::ofstream ofile(path);
        if (!ofile.is_open()) {
            return;
        }

        std::filesystem::path rootPath = path.parent_path();
        std::map<std::string, std::filesystem::path> relativeList;
        for (auto& [name, absPath] : m_fontPaths) {
            auto const relativePath = std::filesystem::relative(absPath, rootPath);
            relativeList.insert(std::make_pair(name, relativePath));
        }

        nlohmann::json json;
        json["fonts"] = relativeList;
        ofile << json;

        m_currentProjectPath = path;
    }

    void FontFactory::ClearFonts() {
        m_fontPaths.clear();
    }

    std::shared_ptr<IFont> FontFactory::GetDefaultFont(int size) {
        return GetFont(m_fontPaths.begin()->first.c_str(), size);
    }

    std::vector<std::string> FontFactory::GetFontNameList() const {
        std::vector<std::string> nameList;
        for (auto& [fontName, fontPath] : m_fontPaths) {
            nameList.push_back(fontName);
        }
        return nameList;
    }

    std::filesystem::path FontFactory::GetFontPath(char const* name) const {
        assert(!m_fontPaths.empty() && "No known fonts.");
        auto const it = m_fontPaths.find(name);
        if (std::end(m_fontPaths) == it) {
            return "";
        }
        return it->second;
    }
}
