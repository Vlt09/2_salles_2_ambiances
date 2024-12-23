#version 330 core

in vec2 fVertexTex;
in vec3 fVertexNormal;

out vec3 fFragColor;

void main() {
    fFragColor = normalize(fVertexNormal);
}