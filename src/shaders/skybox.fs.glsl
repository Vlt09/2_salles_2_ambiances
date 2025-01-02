#version 330 core
out vec4 FragColor;

in vec3 vVertexTex;

uniform samplerCube uskybox;


void main()
{    
    FragColor  = texture(uskybox, vVertexTex);
}