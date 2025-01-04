#version 330 core

in vec2 vVertexTex;
in vec3 vVertexNormal;
in vec3 vVertexPos; // World space
in vec3 vVertexNormalViewSpace;
in vec3 vVertexPosViewSpace;

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

// Material for cell shading
uniform	vec3 uColor;
uniform	float uReflectance;

uniform DirectionalLight uDirectionalLights[1];
uniform PointLight uPointLights[1];
uniform vec3 uAmbientLight;
uniform float uSpecularPower;

uniform vec3 uCameraPos;

uniform bool uIsGlass;

out vec4 fFragColor;


vec4 quantizeColor(vec4 color) {
    color.rgb = floor(color.rgb * 255.0 + 0.5) / 255.0; // Quantize to 8-bit
    return color;
}


vec4 calcLightColour(vec3 light_color, float light_intensity, vec3 position, vec3 to_light_dir, vec3 normal)
{
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specColor = vec4(0, 0, 0, 0);

	// Diffuse Light
	float diffuseFactor = max(dot(normal, to_light_dir), 0.0);

	// Threshold diffuse factor for cartoon effect
	if (diffuseFactor > 0.85)
		diffuseFactor = 1.6;
	else if (diffuseFactor > 0.5)
		diffuseFactor = 1.2;
	else if (diffuseFactor > 0.25)
		diffuseFactor = 0.9;
	else
		diffuseFactor = 0.4;

	diffuseColor = vec4(light_color, 1.0) * light_intensity * diffuseFactor;

	// Specular Light
	vec3 camera_direction = normalize(-position);
	vec3 from_light_dir = -to_light_dir;
	vec3 reflected_light = normalize(reflect(from_light_dir , normal));
	float specularFactor = max( dot(camera_direction, reflected_light), 0.0);
	specularFactor = pow(specularFactor, uSpecularPower);
	specColor = light_intensity  * specularFactor * uReflectance * vec4(light_color, 1.0);

	return (diffuseColor + specColor);
}

vec4 dirLightCellShading(DirectionalLight light, vec3 position, vec3 normal){
    return calcLightColour(light.color, light.intensity, position, normalize(light.direction), normal);
}

vec4 pointLightCellShading(PointLight pointLight, vec3 position, vec3 normal) {
	vec3 light_direction = pointLight.position - position;
	vec3 to_light_dir  = normalize(light_direction);
	vec4 light_colour = calcLightColour(pointLight.color, pointLight.intensity, position, to_light_dir, normal);

	// Apply Attenuation
	float distance = length(light_direction);
	float attenuationInv = pointLight.constant + pointLight.linear * distance +
		pointLight.exponent * distance * distance;
	return light_colour / attenuationInv;
}

void main() {
    vec4 tex = texture(uTexture, vVertexTex);
    vec3 normalColor = normalize(vVertexNormal);

    // fFragColor = tex;
	vec4 baseColour = vec4(uColor, 1);

	vec4 totalLight = vec4(uAmbientLight, 1.0);
	totalLight += dirLightCellShading(uDirectionalLights[0], vVertexPos, vVertexNormal);
	totalLight += pointLightCellShading(uPointLights[0], vVertexPos, vVertexNormal); 

    // fFragColor =  totalLight;

	// Add white outlines
	if (dot(vVertexNormalViewSpace, -vVertexPosViewSpace) <= 0.2) //  Threshold
		fFragColor = vec4(1,1,1,1);
	else
        fFragColor = quantizeColor( (baseColour * totalLight));

    if (uIsGlass){
        fFragColor = tex; 
    }

}