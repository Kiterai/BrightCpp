#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 rect[] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

layout(push_constant) uniform draw_info_t {
    mat3 draw_matrix;
    vec2 screen_size;
    vec2 tex_clip_pos;
    vec2 tex_clip_size;
    vec4 color;
} draw_info;

layout(location = 0) out vec2 fragmentTexCoord;
layout(location = 1) out vec4 fragmentColor;

void main() {
    vec2 pos = vec2(vec3(rect[gl_VertexIndex], 1.0) * draw_info.draw_matrix).xy / (draw_info.screen_size / 2) - 1;

    gl_Position = vec4(pos, 0.0, 1.0);
    fragmentTexCoord = draw_info.tex_clip_pos + draw_info.tex_clip_size * rect[gl_VertexIndex];
    fragmentColor = draw_info.color;
}
