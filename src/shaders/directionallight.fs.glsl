#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;
in vec3 vVertexPos; // World space

uniform sampler2D uTexture;

uniform mat4 uModelMatrix;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightIntensity;
uniform vec3 uLightDir_vs; // viewspace
uniform vec3 uLightPos_vs; 


uniform vec3 uSpotLight;
uniform float uSpotlightCutoff; // Angle limite du spot de lumiere
uniform float uSpotlightExponent;
uniform int uActiveLight;

struct SpotLight{
    vec3 _position, _direction, _lightIntensity;
    vec3 _lightPos; // M space
    float _cutoff, _exponent;
    vec3 m_Kd;
    vec3 m_Ks;
    vec3 m_Ka;

};

uniform SpotLight uSpotLights[2];

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

vec3 pointLightblinnPhong(vec3 normal, SpotLight spotLight) {
    vec3 vFrag_vs = spotLight._position;

    vec3 N = normalize(normal);
    vec3 L = normalize(vFrag_vs - vVertexPos);
    vec3 V = normalize(-vFrag_vs);

    vec3 H = normalize(L + V);

    float dist = distance(vFrag_vs, vVertexPos);
    vec3 attenuation = uLightIntensity / (dist * dist);
    
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = spotLight.m_Kd * diff * uLightIntensity;

    float spec = 0.0;
    if (diff > 0.0) {
        spec = pow(max(dot(N, H), 0.0), uShininess);
    }
    vec3 specular = spotLight.m_Ks * spec * uLightIntensity;

    return diffuse + specular;
}


vec3 spotlightAttenuation(SpotLight spotLight, vec3 normal) {
    vec3 lightToPixel = normalize(spotLight._position - vVertexPos);
    float spotFactor = dot(lightToPixel, normalize(-spotLight._direction));  

    if (isnan(spotFactor)){
        return vec3(255, 255, 255);
    }

    if (spotFactor > spotLight._cutoff) {
        vec3 color = pointLightblinnPhong(normal, spotLight);
        float spotLightIntensity = (1.0 - (1.0 - spotFactor)/(1.0 - spotLight._cutoff));
        return color * spotLightIntensity;
    }

    return vec3(0, 0, 0);//vec3(spotLight.m_Ka * texture(uTexture, vVertexTex).xyz * spotLight.m_Kd);
}

void main()
{
    vec3 tex = texture(uTexture, vVertexTex).xyz;
    vec3 normalColor = normalize(vVertexNormal);
    vec3 border = vec3(12., 0., 0.);

    vec3 lighting = blinnPhong(uKd, normalColor);

    // fFragColor = lighting; 

    vec3 attenuation = vec3(0, 0, 0);
    for (int i = 0 ;i < 2 ;i++) {
        attenuation += spotlightAttenuation(uSpotLights[i], normalColor);
    }
  

    fFragColor = tex + (lighting + attenuation); 


    // fFragColor = normalColor * 0.5 + 0.5;
}
