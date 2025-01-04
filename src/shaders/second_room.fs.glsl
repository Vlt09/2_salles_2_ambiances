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
uniform PointLight uPointLights[50];
uniform vec3 uAmbientLight;
uniform float uSpecularPower;

uniform vec3 uCameraPos;

uniform bool uIsGlass;

out vec4 fFragColor;

//	Simplex 3D Noise 
//	by Ian McEwan, Stefan Gustavson (https://github.com/stegu/webgl-noise)
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}


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

vec4 dirLight(DirectionalLight light, vec3 position, vec3 normal){
    return calcLightColour(light.color, light.intensity, position, normalize(light.direction), normal);
}

vec4 pointLight(PointLight pointLight, vec3 position, vec3 normal) {
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

	float u = clamp(tex.s, 0, 1);
	float v = clamp(tex.t, 0, 1);
	int nRows = 64;

	int row = int(v * float(nRows));
	int col = int(u * float(nRows));

	vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

	float dp = length(vec2(dFdx(u), dFdy(u)));
	float logdp = -log2(dp);
	float ilogdp = floor(logdp);
	float transition = logdp - ilogdp;
	float freq = exp2(ilogdp);

	float sawtooth = fract(u * freq * freq);
	float triangle = abs(2. * sawtooth - 1.);
	float square = step(0.5, triangle);

	vec4 baseColour = vec4(uColor, 1);

	vec4 totalLight = vec4(uAmbientLight, 1.0);
	totalLight += dirLight(uDirectionalLights[0], vVertexPos, vVertexNormal);
    for (int i = 0; i < 50; i++){
    	totalLight += pointLight(uPointLights[i], vVertexPos, vVertexNormal); 
    }

    if (uIsGlass){
        fFragColor =  tex * totalLight;
    }
    else{
        fFragColor = (vec4(vec3(vec3(1., 0., 1.) * square), 1.0) / snoise(vVertexPos)) + totalLight;

    }


}