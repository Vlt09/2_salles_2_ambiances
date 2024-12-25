#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;

uniform sampler2D uTexture;

out vec3 fFragColor;

void main() {
    vec3 tex = texture(uTexture, vVertexTex).xyz;
    fFragColor = tex;
}