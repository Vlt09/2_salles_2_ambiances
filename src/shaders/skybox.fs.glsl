#version 330 core
out vec4 FragColor;

in vec3 vVertexTex;

uniform samplerCube uskybox;


void main()
{        
    vec3 skyboxColor = texture(uskybox, vVertexTex).rgb;
    FragColor = vec4(skyboxColor * vec3(0.3,0.3, 0.3 ), 1.0);
    // FragColor  = texture(uskybox, vVertexTex);
}