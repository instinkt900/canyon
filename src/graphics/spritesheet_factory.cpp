#include "common.h"
#include "moth_graphics/graphics/spritesheet_factory.h"
#include "moth_graphics/graphics/spritesheet.h"

#include <optional>

namespace moth_graphics::graphics {
namespace {

SpriteSheet::LoopType ParseLoopType(std::string const& loopStr) {
    if (loopStr == "loop") {
        return SpriteSheet::LoopType::Loop;
    }
    if (loopStr == "reset") {
        return SpriteSheet::LoopType::Reset;
    }
    return SpriteSheet::LoopType::Stop;
}

std::optional<std::vector<SpriteSheet::FrameEntry>> ParseFrames(
    nlohmann::json const& framesJson, std::string const& path) {
    std::vector<SpriteSheet::FrameEntry> frames;
    for (auto const& frameJson : framesJson) {
        if (!frameJson.contains("x") || !frameJson.contains("y") ||
            !frameJson.contains("w") || !frameJson.contains("h")) {
            spdlog::error("SpriteSheetFactory: '{}' frame {} missing x/y/w/h fields — aborting",
                          path, frameJson.dump());
            return std::nullopt;
        }
        if (!frameJson["x"].is_number_integer() || !frameJson["y"].is_number_integer() ||
            !frameJson["w"].is_number_integer() || !frameJson["h"].is_number_integer()) {
            spdlog::error("SpriteSheetFactory: '{}' frame {} x/y/w/h must be integers — aborting",
                          path, frameJson.dump());
            return std::nullopt;
        }
        int const x = frameJson["x"].get<int>();
        int const y = frameJson["y"].get<int>();
        int const w = frameJson["w"].get<int>();
        int const h = frameJson["h"].get<int>();
        if (w <= 0 || h <= 0) {
            spdlog::error("SpriteSheetFactory: '{}' frame {} has non-positive w/h ({},{}) — aborting",
                          path, frameJson.dump(), w, h);
            return std::nullopt;
        }
        SpriteSheet::FrameEntry entry;
        entry.rect = MakeRect(x, y, w, h);
        entry.pivot.x = frameJson.value("pivot_x", 0);
        entry.pivot.y = frameJson.value("pivot_y", 0);
        frames.push_back(entry);
    }
    return frames;
}

std::optional<SpriteSheet::ClipEntry> ParseClipEntry(
    nlohmann::json const& clipJson, std::string const& path, int totalFrames) {
    if (!clipJson.contains("name") || !clipJson["name"].is_string()) {
        spdlog::warn("SpriteSheetFactory: '{}' skipping clip with missing or non-string 'name'",
                     path);
        return std::nullopt;
    }
    if (!clipJson.contains("frames") || !clipJson["frames"].is_array()) {
        spdlog::warn("SpriteSheetFactory: '{}' skipping clip '{}': missing 'frames' array",
                     path, clipJson["name"].get<std::string>());
        return std::nullopt;
    }

    SpriteSheet::ClipEntry entry;
    entry.name = clipJson["name"].get<std::string>();

    bool valid = true;
    for (auto const& stepJson : clipJson["frames"]) {
        if (!stepJson.contains("frame") || !stepJson.contains("duration_ms")) {
            spdlog::warn("SpriteSheetFactory: '{}' clip '{}': step missing frame/duration_ms",
                         path, entry.name);
            valid = false;
            break;
        }
        if (!stepJson["frame"].is_number_integer() || !stepJson["duration_ms"].is_number_integer()) {
            spdlog::warn("SpriteSheetFactory: '{}' clip '{}': frame/duration_ms must be integers",
                         path, entry.name);
            valid = false;
            break;
        }
        SpriteSheet::ClipFrame step;
        step.frameIndex = stepJson["frame"].get<int>();
        step.durationMs = stepJson["duration_ms"].get<int>();
        if (step.frameIndex < 0 || step.frameIndex >= totalFrames) {
            spdlog::warn("SpriteSheetFactory: '{}' clip '{}': frame index {} out of range [0, {})",
                         path, entry.name, step.frameIndex, totalFrames);
            valid = false;
            break;
        }
        if (step.durationMs <= 0) {
            spdlog::warn("SpriteSheetFactory: '{}' clip '{}': duration_ms must be > 0 (got {})",
                         path, entry.name, step.durationMs);
            valid = false;
            break;
        }
        entry.desc.frames.push_back(step);
    }

    if (!valid || entry.desc.frames.empty()) {
        spdlog::warn("SpriteSheetFactory: '{}' skipping empty or invalid clip '{}'",
                     path, entry.name);
        return std::nullopt;
    }

    std::string loopStr = "stop";
    if (clipJson.contains("loop") && clipJson["loop"].is_string()) {
        loopStr = clipJson["loop"].get<std::string>();
    }
    entry.desc.loop = ParseLoopType(loopStr);

    return entry;
}

std::vector<SpriteSheet::ClipEntry> ParseClips(
    nlohmann::json const& clipsJson, std::string const& path, int totalFrames) {
    std::vector<SpriteSheet::ClipEntry> clips;
    for (auto const& clipJson : clipsJson) {
        auto entry = ParseClipEntry(clipJson, path, totalFrames);
        if (entry) {
            clips.push_back(std::move(*entry));
        }
    }
    return clips;
}

} // namespace

    SpriteSheetFactory::SpriteSheetFactory(AssetContext& context)
        : m_context(context) {
    }

    void SpriteSheetFactory::FlushCache() {
        m_cache.clear();
    }

    std::shared_ptr<SpriteSheet> SpriteSheetFactory::GetSpriteSheet(std::filesystem::path const& path) {
        std::error_code ec;
        auto const absPath = std::filesystem::absolute(path, ec);
        if (ec) {
            spdlog::error("SpriteSheetFactory: failed to resolve path '{}': {}", path.string(), ec.message());
            return nullptr;
        }
        auto const key = absPath.lexically_normal().string();

        auto const cacheIt = m_cache.find(key);
        if (cacheIt != std::end(m_cache)) {
            return cacheIt->second;
        }

        bool const fileExists = std::filesystem::exists(absPath, ec);
        if (ec) {
            spdlog::error("SpriteSheetFactory: failed to check existence of '{}': {}", path.string(), ec.message());
            return nullptr;
        }
        if (!fileExists) {
            spdlog::error("SpriteSheetFactory: '{}' does not exist", path.string());
            return nullptr;
        }

        std::ifstream ifile(absPath);
        if (!ifile.is_open()) {
            spdlog::error("SpriteSheetFactory: failed to open '{}'", absPath.string());
            return nullptr;
        }

        nlohmann::json json;
        try {
            ifile >> json;
        } catch (std::exception const& e) {
            spdlog::error("SpriteSheetFactory: failed to parse '{}': {}", path.string(), e.what());
            return nullptr;
        }

        if (!json.contains("image") || !json["image"].is_string()) {
            spdlog::error("SpriteSheetFactory: '{}' missing 'image' string field", path.string());
            return nullptr;
        }
        if (!json.contains("frames") || !json["frames"].is_array()) {
            spdlog::error("SpriteSheetFactory: '{}' missing 'frames' array", path.string());
            return nullptr;
        }

        ec = {};
        auto const rootPath = path.parent_path();
        auto const imageAbsPath = std::filesystem::absolute(
            rootPath / json["image"].get<std::string>(), ec).lexically_normal();
        if (ec) {
            spdlog::error("SpriteSheetFactory: '{}' failed to resolve image path: {}", path.string(), ec.message());
            return nullptr;
        }

        std::shared_ptr<ITexture> texture(m_context.TextureFromFile(imageAbsPath));
        if (!texture) {
            spdlog::error("SpriteSheetFactory: '{}' failed to load image '{}'",
                          path.string(), imageAbsPath.string());
            return nullptr;
        }
        Image image(texture);

        auto frames = ParseFrames(json["frames"], path.string());
        if (!frames) {
            return nullptr;
        }
        if (frames->empty()) {
            spdlog::error("SpriteSheetFactory: '{}' frames array is empty", path.string());
            return nullptr;
        }

        int const totalFrames = static_cast<int>(frames->size());

        std::vector<SpriteSheet::ClipEntry> clips;
        if (json.contains("clips") && json["clips"].is_array()) {
            clips = ParseClips(json["clips"], path.string(), totalFrames);
        }

        auto sheet = std::make_shared<SpriteSheet>(std::move(image), std::move(*frames), std::move(clips));
        m_cache.insert({ key, sheet });
        return sheet;
    }
}
