#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 rect[] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

layout(location = 0) out vec2 fragmentTexCoord;

void main() {
    gl_Position = vec4(rect[gl_VertexIndex], 0.0, 1.0);
    fragmentTexCoord = rect[gl_VertexIndex];
}
