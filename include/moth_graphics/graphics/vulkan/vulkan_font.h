#pragma once

#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/vulkan/vulkan_shader.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "moth_graphics/graphics/vulkan/vulkan_buffer.h"
#include "moth_graphics/graphics/vulkan/vulkan_texture.h"
#include "moth_graphics/utils/vector.h"

#include <harfbuzz/hb.h>
#include <vulkan/vulkan_core.h>

// Forward-declare FT_Face to avoid requiring FreeType headers in consumers.
// FT_Face is typedef struct FT_FaceRec_ *FT_Face in freetype/freetype.h.
struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace moth_graphics::graphics::vulkan {
    class Font : public IFont {
    public:
        static std::unique_ptr<Font> Load(std::filesystem::path const& path, int size, SurfaceContext& context);
        virtual ~Font();

        IntVec2 Measure(std::string_view text) const override;

        int32_t GetLineHeight() const override { return m_lineHeight; }
        int32_t GetAscent() const override { return m_ascent; }
        int32_t GetDescent() const override { return m_descent; }
        int32_t GetUnderline() const { return m_underline; }

        IntVec2 const& GetGlyphBearing(int glyphIndex) const;

        int32_t GetStringWidth(std::string_view const& str) const;
        int32_t GetColumnHeight(std::string const& str, int32_t width) const;

        struct ShapedInfo {
            int glyphIndex;
            IntVec2 advance;
            IntVec2 offset;
        };
        std::vector<ShapedInfo> ShapeString(std::string_view const& str) const;

        struct LineDesc {
            int32_t lineWidth;
            std::string_view text;
        };
        std::vector<LineDesc> WrapString(std::string const& str, int32_t width) const;

        VkDescriptorSet GetVKDescriptorSetForShader(Shader const& shader);
        // VkDescriptorSet GetVKDescriptorSet() const { return m_vkDescriptorSet; }

    private:
        Font(FT_Face face, int size, SurfaceContext& context);

        int CodepointToIndex(int codepoint) const;

        SurfaceContext& m_context;

        // harfbuzz stuff. this should probably be wrapped
        hb_font_t* m_hbFont = nullptr;
        mutable hb_buffer_t* m_hbBuffer = nullptr;

        // texture containing all glyphs
        std::unique_ptr<Texture> m_glyphAtlas;

        // global font measurements
        int32_t m_lineHeight;
        int32_t m_ascent;
        int32_t m_descent;
        int32_t m_underline;

        struct ShaderInfo {
            IntVec2 Size;
            IntVec2 Unused;
            FloatVec2 UV0;
            FloatVec2 UV1;
        };

        std::map<int, uint32_t> m_codepointToAtlasIndex; //
        std::vector<IntVec2> m_glyphBearings;            // glyph bearing values
        std::vector<ShaderInfo> m_shaderInfos;           // glyph info specifically for the shader

        std::map<uint32_t, VkDescriptorSet> m_vkDescriptorSets;
        // VkDescriptorSet m_vkDescriptorSet;
        std::unique_ptr<Buffer> m_glyphInfosBuffer; // the buffer that stores the glyph infos
    };
}
