#version 410 core

uniform struct Textures {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
} u_textures;

in VertexData {
    vec2 uvs;
} i;

out vec4 out_color;

void main() {
    out_color = texture(u_textures.diffuse, i.uvs);
}
