#version 330 core

layout(location = 1) in vec3 aVertexPosition;
layout(location = 2) in vec3 aVertexNormal;
layout(location = 3) in vec2 aVertexTex;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec2 vVertexTex;
out vec3 vVertexNormal;

void main() {
    vVertexTex = aVertexTex;
    vVertexNormal = aVertexNormal;
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1);
};