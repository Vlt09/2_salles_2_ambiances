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

vec3 blinnPhong(vec3 Kd, vec3 N) {
    vec3 L = normalize(-uLightDir_vs);
    vec3 V = normalize(-vVertexPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * uLightIntensity * NdotL;

    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess);

    return diffuse + specular;
}

vec3 pointLightblinnPhong(vec3 normal, vec3 fragPos_vs) {
    vec3 N = normalize(normal);
    vec3 L = normalize(fragPos_vs - vVertexPos);
    vec3 V = normalize(-fragPos_vs);

    vec3 H = normalize(L + V);

    float dist = distance(fragPos_vs, vVertexPos);
    vec3 attenuation = uLightIntensity / (dist * dist);
    
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
    vec3 lightToPixel = normalize(vVertexPos - spotLight);
    vec3 dir = vec3(0,-1,0);
    float spotFactor = dot(lightToPixel, dir);  

    if (isnan(spotFactor)){
        return vec3(255, 255, 255);
    }

    if (spotFactor > uSpotlightCutoff) {
        vec3 color = pointLightblinnPhong(normal, uLightPos_vs);
        float spotLightIntensity = (1.0 - (1.0 - spotFactor)/(1.0 - uSpotlightCutoff));
        return color * spotLightIntensity;
    }

    return vec3(0, 0, 0);
}

void main()
{
    vec3 tex = texture(uTexture, vVertexTex).xyz;
    vec3 normalColor = normalize(vVertexNormal);   

    vec3 lighting = blinnPhong(uKd, normalColor);

    // fFragColor = lighting; 
    
    vec3 attenuation = spotlightAttenuation(uSpotLight, normalColor);
    

    fFragColor = (attenuation); 


    // fFragColor = normalColor * 0.5 + 0.5;
}
