#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;

uniform sampler2D uTexture;

out vec4 fFragColor;

void main() {
    vec4 tex = texture(uTexture, vVertexTex);
    vec3 normalColor = normalize(vVertexNormal);

    fFragColor = tex;
}