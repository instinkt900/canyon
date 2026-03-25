#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex
{
    vec4 gl_Position;
};

struct GlyphInfo
{
    uvec2 glyph_size;
    uvec2 unused;
    vec4 uv;
};

layout (set = 0, binding = 0) readonly buffer GlyphBuffer
{
	GlyphInfo glyphs[];
} glyph_buffer;

layout(push_constant) uniform uPushConstant
{
    vec2 xyScale;
    vec2 xyOffset;
} pc;

layout(location = 0) in vec2 inPos;
layout(location = 1) in uint inGlyphIndex;
layout(location = 2) in float inRotation;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoord;

void main()
{
    GlyphInfo gi = glyph_buffer.glyphs[inGlyphIndex];

    vec2 corners[4] = vec2[](
        vec2(0.0, 0.0),             vec2(gi.glyph_size.x,   0.0),
        vec2(0.0, gi.glyph_size.y), vec2(gi.glyph_size.x,   gi.glyph_size.y)
    );

    vec2 uv[4] = vec2[](
        vec2(gi.uv.x, gi.uv.y),
        vec2(gi.uv.z, gi.uv.y),
        vec2(gi.uv.x, gi.uv.w),
        vec2(gi.uv.z, gi.uv.w)
    );

    // Rotate the local glyph corner offset around (0,0) then translate to inPos.
    float c = cos(inRotation);
    float s = sin(inRotation);
    vec2 localOffset = corners[gl_VertexIndex];
    vec2 rotatedOffset = vec2(c * localOffset.x - s * localOffset.y,
                              s * localOffset.x + c * localOffset.y);

    gl_Position = vec4((rotatedOffset + inPos) * pc.xyScale + pc.xyOffset, 0.0, 1.0);
    outColor = inColor;
    outTexCoord = uv[gl_VertexIndex];
}
