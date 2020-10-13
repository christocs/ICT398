#version 410 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uvs;
layout (location = 3) in ivec4 in_bone_index;
layout (location = 4) in vec4 in_bone_weight;

const int MAX_BONES = 100;

uniform struct Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
} u_matrices;

uniform mat4 u_bones[MAX_BONES];

out VertexData {
    vec2 uvs;
} o;

void main() {
    o.uvs = in_uvs;
    gl_Position = u_matrices.projection * u_matrices.view * u_matrices.model * vec4(in_pos, 1.0);
}
