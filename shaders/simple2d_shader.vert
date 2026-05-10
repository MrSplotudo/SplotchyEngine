#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;    // unused
layout(location = 3) in vec2 uv;        // unused

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push{
    mat4 transform;
    mat4 modelMatrix;  // unused
} push;

void main() {
    gl_Position = push.transform * vec4(position, 1.0);
    fragColor = color;
}