#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;
in vec3 vVertexPos;

uniform sampler2D uTexture;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightIntensity;
uniform vec3 uLightDir_vs; // viewspace

out vec3 fFragColor;

vec3 blinnPhong(){

    vec3 N = normalize(vVertexNormal);
    vec3 w_i = normalize(uLightDir_vs);
    vec3 w0 = normalize(-vVertexPos);

    // Halway vector
    vec3 H = normalize(w_i + w0);

    float diff = max(dot(N, w_i), 0.0);
    vec3 diffuse = uKd * diff * uLightIntensity;

    float spec = 0.0;
    if (diff > 0.0){
        spec = pow(max(dot(N, H), 0.0), uShininess);
    }
    vec3 specular = uKs * spec * uLightIntensity;

    return diffuse + specular;
}

void main()
{
    vec3 tex = texture(uTexture, vVertexTex).xyz;

    vec3 lighting = blinnPhong();

    fFragColor = lighting + tex;
} 