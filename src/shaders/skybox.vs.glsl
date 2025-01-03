#version 330 core
layout (location = 0) in vec3 aVertexPosition;

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

out vec3 vVertexTex;
out vec3 vVertexPos;


void main()
{
    vVertexTex = aVertexPosition;
    // vVertexPos = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);
}