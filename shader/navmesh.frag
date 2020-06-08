#version 410 core

uniform struct Textures {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
} u_textures;

in VertexData {
    vec2 uvs;
    vec3 pos;
} i;

out vec4 out_color;

void main() {
    out_color = vec4(0, 255, 255, 1);
}
