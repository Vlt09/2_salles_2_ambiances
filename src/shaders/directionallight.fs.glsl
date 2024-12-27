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
uniform vec3 uLightPos_vs; 


uniform vec3 uSpotLight;
uniform float uSpotlightCutoff; // Angle limite du spot de lumiere
uniform float uSpotlightExponent;

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

vec3 pointLightblinnPhong(vec3 normal, vec3 fragPos_vs) {
    vec3 N = normalize(normal);
    vec3 L = normalize(uLightPos_vs - vVertexPos);
    vec3 V = normalize(-fragPos_vs);

    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = uKd * diff * uLightIntensity;

    float spec = 0.0;
    if (diff > 0.0) {
        spec = pow(max(dot(N, H), 0.0), uShininess);
    }
    vec3 specular = uKs * spec * uLightIntensity;

    return diffuse + specular;
}

vec3 spotlightAttenuation(vec3 spotLight, vec3 normal) {
    vec3 dir = vec3(0, -1, 0);
    vec3 lightToPixel = normalize(vVertexPos - spotLight);
    float spotFactor = dot(lightToPixel, normalize(dir));

    if (spotFactor > uSpotlightCutoff) {
        vec3 color = pointLightblinnPhong(spotLight, normal);
        float spotLightIntensity = (1.0 - (1.0 - spotFactor)/(1.0 - uSpotlightCutoff));
        return color * spotLightIntensity;
    }
    return vec3(0, 0, 0);
}

void main()
{
    vec3 tex = texture(uTexture, vVertexTex).xyz;
    vec3 normalColor = normalize(vVertexNormal);

    vec3 attenuation = spotlightAttenuation(uSpotLight, vVertexNormal);
    vec3 lighting = blinnPhong();

    lighting += attenuation;

    fFragColor = lighting + normalColor;
} 