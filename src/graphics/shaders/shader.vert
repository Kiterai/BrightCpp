#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform draw_info_t {
    mat3 draw_matrix;
    vec2 screen_size;
    vec2 tex_clip_pos;
    vec2 tex_clip_size;
    vec4 color;
} draw_info;

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragmentTexCoord;
layout(location = 1) out vec4 fragmentColor;

void main() {
    vec2 pos = vec2(vec3(inPos, 1.0) * draw_info.draw_matrix).xy / (draw_info.screen_size / 2) - 1;

    gl_Position = vec4(pos, 0.0, 1.0);
    fragmentTexCoord = draw_info.tex_clip_pos + draw_info.tex_clip_size * inTexCoord;
    fragmentColor = draw_info.color;
}
