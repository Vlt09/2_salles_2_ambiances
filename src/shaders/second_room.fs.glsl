#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;

uniform sampler2D uTexture;

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float intensity;
};

struct PointLight
{
	vec3 color;
	vec3 position; // in world space
	float intensity;

    float constant;
	float linear;
	float exponent;
};

uniform DirectionalLight uDirectionalLights[1];
uniform PointLight uPointLights[1];


out vec4 fFragColor;

// vec3 pointLightblinnPhong(vec3 normal, SpotLight spotLight) {
//     vec3 vFrag_vs = spotLight._position;

//     vec3 N = normalize(normal);
//     vec3 L = normalize(vFrag_vs - vVertexPos);
//     vec3 V = normalize(-vFrag_vs);

//     vec3 H = normalize(L + V);

//     float dist = distance(vFrag_vs, vVertexPos);
//     vec3 attenuation = uLightIntensity / (dist * dist);
    
//     float diff = max(dot(N, L), 0.0);
//     vec3 diffuse = spotLight.m_Kd * diff * uLightIntensity;

//     float spec = 0.0;
//     if (diff > 0.0) {
//         spec = pow(max(dot(N, H), 0.0), uShininess);
//     }
//     vec3 specular = spotLight.m_Ks * spec * uLightIntensity;

//     return diffuse + specular;
// }




void main() {
    vec4 tex = texture(uTexture, vVertexTex);
    vec3 normalColor = normalize(vVertexNormal);

    fFragColor = tex;
}