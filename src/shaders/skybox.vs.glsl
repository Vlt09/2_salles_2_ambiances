#version 330 core
layout (location = 0) in vec3 aVertexPosition;

out vec3 vVertexTex;

uniform mat4 uMVPMatrix;

void main()
{
    vVertexTex = aVertexPosition;
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);
}