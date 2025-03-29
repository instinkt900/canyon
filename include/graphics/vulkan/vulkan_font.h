#pragma once

#include "graphics/ifont.h"
#include "graphics/vulkan/vulkan_shader.h"
#include "vulkan_buffer.h"

#include "harfbuzz/hb.h"

namespace graphics::vulkan {
    class Font : public IFont {
    public:
        static std::unique_ptr<Font> Load(std::filesystem::path const& path, int size, SurfaceContext& context);
        virtual ~Font();

        int32_t GetLineHeight() const { return m_lineHeight; }
        int32_t GetAscent() const { return m_ascent; }
        int32_t GetDescent() const { return m_descent; }
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
