#include "common.h"
#include "moth_graphics/graphics/spritesheet_factory.h"
#include "moth_graphics/graphics/spritesheet.h"

//NOLINTBEGIN(readability-function-cognitive-complexity)

namespace moth_graphics::graphics {
    SpriteSheetFactory::SpriteSheetFactory(AssetContext& context)
        : m_context(context) {
    }

    void SpriteSheetFactory::FlushCache() {
        m_cache.clear();
    }

    std::shared_ptr<SpriteSheet> SpriteSheetFactory::GetSpriteSheet(std::filesystem::path const& path) {
        auto const key = std::filesystem::absolute(path).lexically_normal().string();

        auto const cacheIt = m_cache.find(key);
        if (cacheIt != std::end(m_cache)) {
            return cacheIt->second;
        }

        if (!std::filesystem::exists(path)) {
            spdlog::error("SpriteSheetFactory: '{}' does not exist", path.string());
            return nullptr;
        }

        std::ifstream ifile(path);
        if (!ifile.is_open()) {
            spdlog::error("SpriteSheetFactory: failed to open '{}'", path.string());
            return nullptr;
        }

        nlohmann::json json;
        try {
            ifile >> json;
        } catch (std::exception const& e) {
            spdlog::error("SpriteSheetFactory: failed to parse '{}': {}", path.string(), e.what());
            return nullptr;
        }

        auto const rootPath = path.parent_path();

        if (!json.contains("image") || !json["image"].is_string()) {
            spdlog::error("SpriteSheetFactory: '{}' missing 'image' string field", path.string());
            return nullptr;
        }
        if (!json.contains("frame_width") || !json.contains("frame_height") ||
            !json.contains("frame_cols")  || !json.contains("frame_rows")   ||
            !json.contains("max_frames")) {
            spdlog::error("SpriteSheetFactory: '{}' missing required frame layout fields", path.string());
            return nullptr;
        }
        if (!json["frame_width"].is_number_integer() || !json["frame_height"].is_number_integer() ||
            !json["frame_cols"].is_number_integer()  || !json["frame_rows"].is_number_integer()   ||
            !json["max_frames"].is_number_integer()) {
            spdlog::error("SpriteSheetFactory: '{}' frame layout fields must be integers", path.string());
            return nullptr;
        }

        auto const imageAbsPath = std::filesystem::absolute(
            rootPath / json["image"].get<std::string>()).lexically_normal();

        auto image = m_context.ImageFromFile(imageAbsPath);
        if (!image) {
            spdlog::error("SpriteSheetFactory: '{}' failed to load image '{}'",
                          path.string(), imageAbsPath.string());
            return nullptr;
        }
        auto sharedImage = std::shared_ptr<IImage>(std::move(image));

        SpriteSheet::SheetDesc sheetDesc;
        sheetDesc.FrameDimensions.x = json["frame_width"].get<int>();
        sheetDesc.FrameDimensions.y = json["frame_height"].get<int>();
        sheetDesc.SheetCells.x      = json["frame_cols"].get<int>();
        sheetDesc.SheetCells.y      = json["frame_rows"].get<int>();
        sheetDesc.MaxFrames         = json["max_frames"].get<int>();

        if (sheetDesc.FrameDimensions.x <= 0) {
            spdlog::error("SpriteSheetFactory: '{}' frame_width must be > 0 (got {})", path.string(), sheetDesc.FrameDimensions.x);
            return nullptr;
        }
        if (sheetDesc.FrameDimensions.y <= 0) {
            spdlog::error("SpriteSheetFactory: '{}' frame_height must be > 0 (got {})", path.string(), sheetDesc.FrameDimensions.y);
            return nullptr;
        }
        if (sheetDesc.SheetCells.x <= 0) {
            spdlog::error("SpriteSheetFactory: '{}' frame_cols must be > 0 (got {})", path.string(), sheetDesc.SheetCells.x);
            return nullptr;
        }
        if (sheetDesc.SheetCells.y <= 0) {
            spdlog::error("SpriteSheetFactory: '{}' frame_rows must be > 0 (got {})", path.string(), sheetDesc.SheetCells.y);
            return nullptr;
        }
        if (sheetDesc.MaxFrames < 0) {
            spdlog::error("SpriteSheetFactory: '{}' max_frames must be >= 0 (got {})", path.string(), sheetDesc.MaxFrames);
            return nullptr;
        }

        int const totalFrames = sheetDesc.MaxFrames > 0
            ? sheetDesc.MaxFrames
            : sheetDesc.SheetCells.x * sheetDesc.SheetCells.y;

        std::vector<SpriteSheet::ClipEntry> clips;
        if (json.contains("clips") && json["clips"].is_array()) {
            for (auto const& clipJson : json["clips"]) {
                if (!clipJson.contains("name") || !clipJson.contains("start") ||
                    !clipJson.contains("end")  || !clipJson.contains("fps")) {
                    spdlog::warn("SpriteSheetFactory: '{}' skipping clip with missing fields",
                                 path.string());
                    continue;
                }
                if (!clipJson["name"].is_string()) {
                    spdlog::warn("SpriteSheetFactory: '{}' skipping clip: 'name' must be a string", path.string());
                    continue;
                }
                if (!clipJson["start"].is_number_integer() || !clipJson["end"].is_number_integer() ||
                    !clipJson["fps"].is_number_integer()) {
                    spdlog::warn("SpriteSheetFactory: '{}' skipping clip '{}': start/end/fps must be integers",
                                 path.string(), clipJson["name"].get<std::string>());
                    continue;
                }

                SpriteSheet::ClipEntry entry;
                entry.name       = clipJson["name"].get<std::string>();
                entry.desc.Start = clipJson["start"].get<int>();
                entry.desc.End   = clipJson["end"].get<int>();
                entry.desc.FPS   = clipJson["fps"].get<int>();

                if (entry.desc.Start < 0 || entry.desc.End < entry.desc.Start || entry.desc.End >= totalFrames) {
                    spdlog::warn("SpriteSheetFactory: '{}' skipping clip '{}': start={} end={} out of range [0, {})",
                                 path.string(), entry.name, entry.desc.Start, entry.desc.End, totalFrames);
                    continue;
                }
                if (entry.desc.FPS <= 0) {
                    spdlog::warn("SpriteSheetFactory: '{}' skipping clip '{}': fps must be > 0 (got {})",
                                 path.string(), entry.name, entry.desc.FPS);
                    continue;
                }

                std::string loopStr = "loop";
                if (clipJson.contains("loop") && clipJson["loop"].is_string()) {
                    loopStr = clipJson["loop"].get<std::string>();
                }
                if (loopStr == "stop") {
                    entry.desc.Loop = SpriteSheet::LoopType::Stop;
                } else if (loopStr == "reset") {
                    entry.desc.Loop = SpriteSheet::LoopType::Reset;
                } else {
                    entry.desc.Loop = SpriteSheet::LoopType::Loop;
                }

                clips.push_back(std::move(entry));
            }
        }

        auto sheet = std::make_shared<SpriteSheet>(sharedImage, sheetDesc, std::move(clips));
        m_cache.insert({ key, sheet });
        return sheet;
    }
}

//NOLINTEND(readability-function-cognitive-complexity)
