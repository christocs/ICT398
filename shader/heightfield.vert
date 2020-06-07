#version 410 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uvs;

uniform struct Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
} u_matrices;

out VertexData {
    vec2 uvs;
    vec3 pos;
} o;

void main() {
    o.uvs = in_uvs;
    o.pos = in_pos;
    gl_Position = u_matrices.projection * u_matrices.view * u_matrices.model * vec4(in_pos, 1.0);
}
