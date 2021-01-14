#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec2 uvs;
out vec3 normal;
out vec3 fragPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	uvs = aTextCoord;
	fragPos = vec3(model * vec4(aPosition, 1.0)); // lightning calculation need fragment coordinates in world space
	normal = mat3(normalMatrix) * aNormal; 
}

#shader fragment
#version 330 core

in vec2 uvs;
in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

uniform vec4 uColor;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform float uOffsetX;
uniform float uOffsetY;
uniform float uTileX;
uniform float uTileY;

uniform vec4 uSpecularColor;
uniform float uSpecularShininess;

uniform bool blinn;

//LIGHT
uniform vec3 uViewPosition;

struct DirectionalLight {
	 vec4 lightColor;
	 vec3 lightDirection;
	 float lightIntensity;
};

struct PointLight {
	vec4 lightColor;
	vec3 lightPosition;
	float lightIntensity;
	vec3 attenuationConstant; // x = 1/Rmax^2 y= 2/Rmax y=Rmax
};

struct SpotLight {
	vec4 lightColor;
	vec3 lightPosition;
	vec3 lightDirection;
	float lightIntensity;
	vec3 attenuationConstant; // x = 1/Rmax^2 y= 2/Rmax y=Rmax
	vec2 cutOff; // x = outer  y = inner
};

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;
uniform SpotLight uSpotLight;

uniform vec4 uAmbientColor;
uniform float uAmbientIntensity;

vec4 PointLightCalc(vec2 uvs)
{	
	//Attenuation
	// Based on Eric Lengyel's Game engine development Vol 2 Smooth Attenuation Equation
	float distance = length(uPointLight.lightPosition - fragPos);
	float distance2 = distance * distance;
	// Constants in vec3 attenuationConstant are 1/Rmax^2, 2/Rmax, Rmax
	// Attenuation goes from 1 to 0 at Rmax. Then start to grow so we must set to 0 if R > Rmax	
	float attenuation = (distance2 * uPointLight.attenuationConstant.x * (sqrt(distance2) * uPointLight.attenuationConstant.y - 3.0) + 1);
	// Distance > Range --> attenuation = 0;
	attenuation = attenuation * int(uPointLight.attenuationConstant.z > distance);
	
	// Ambient not affected by attenuation
	vec4 ambientComponent = texture(diffuseTexture, uvs) * uAmbientColor * uAmbientIntensity;
	
	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(uPointLight.lightPosition - fragPos);
	float diffuseAmount = max(dot(norm, lightDir), 0.0);
	vec4 diffuseComponent = texture(diffuseTexture, uvs) * diffuseAmount * uPointLight.lightColor * uPointLight.lightIntensity * attenuation;

	//Specular
	// use if Phong model
	vec3 reflectLight = reflect(-lightDir, norm);
	vec3 viewDir = normalize(uViewPosition - fragPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float specularAmount = 0;
	if (blinn)
	{
		specularAmount = pow(max(dot(normal, halfwayDir), 0.0), uSpecularShininess);
	}
	else
	{
		specularAmount = pow(max(dot(viewDir, reflectLight), 0.0), uSpecularShininess);
	}

	vec4 specularComponent = texture(specularTexture, uvs) * specularAmount * uSpecularColor * uPointLight.lightColor * uPointLight.lightIntensity * attenuation;

	vec4 lightTotal = ambientComponent + diffuseComponent + specularComponent;

	return lightTotal;
}

vec4 DirectionalLightCal(vec2 uvs)
{
	// Ambient
	vec4 ambientComponent = texture(diffuseTexture, uvs) * uAmbientColor * uAmbientIntensity;

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-uDirectionalLight.lightDirection);
	float diffuseAmount = max(dot(norm, lightDir), 0.0);
	vec4 diffuseComponent = texture(diffuseTexture, uvs) * diffuseAmount * uDirectionalLight.lightColor * uDirectionalLight.lightIntensity;

	//Specular
	// use if Phong model
	vec3 reflectLight = reflect(-lightDir, norm);
	vec3 viewDir = normalize(uViewPosition - fragPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float specularAmount = 0;
	if (blinn)
	{
		specularAmount = pow(max(dot(normal, halfwayDir), 0.0), uSpecularShininess);
	}
	else
	{
		specularAmount = pow(max(dot(viewDir, reflectLight), 0.0), uSpecularShininess);
	}

	vec4 specularComponent = texture(specularTexture, uvs) * specularAmount * uSpecularColor * uDirectionalLight.lightColor * uDirectionalLight.lightIntensity;

	vec4 lightTotal = ambientComponent + diffuseComponent + specularComponent;

	return lightTotal;
}

vec4 SpotLightCal(vec2 uvs)
{	
	//AttenuationByRange
	// Based on Eric Lengyel's Game engine development Vol 2 Smooth Attenuation Equation
	float distance = length(uSpotLight.lightPosition - fragPos);
	float distance2 = distance * distance;
	// Attenuation goes from 1 to 0 at Rmax. Then start to grow so we must set to 0 if R > Rmax	
	float attenuationByRange = (distance2 * uSpotLight.attenuationConstant.x * (sqrt(distance2) * uSpotLight.attenuationConstant.y - 3.0) + 1);
	attenuationByRange = attenuationByRange *  int(uSpotLight.attenuationConstant.z > distance);
	
	vec3 lightDir = normalize(uSpotLight.lightPosition - fragPos);
	
	//Attenuation by Angle
	float theta = dot(lightDir, -uSpotLight.lightDirection);
	float epsilon = uSpotLight.cutOff.y - uSpotLight.cutOff.x;
	float attenuationByAngle = clamp((theta - uSpotLight.cutOff.x) / epsilon, 0.0, 1.0);
	// Attenuation by angle corrected by attenuation by distance
	float attenuation = attenuationByAngle * attenuationByRange;

	// Ambient
	vec4 ambientComponent = texture(diffuseTexture, uvs) * uAmbientColor * uAmbientIntensity;

	// Diffuse
	vec3 norm = normalize(normal);	
	float diffuseAmount = max(dot(norm, lightDir), 0.0);
	vec4 diffuseComponent = texture(diffuseTexture, uvs) * diffuseAmount * uSpotLight.lightColor * uSpotLight.lightIntensity * attenuation;

	//Specular
	// use if Phong model
	vec3 reflectLight = reflect(-lightDir, norm);
	vec3 viewDir = normalize(uViewPosition - fragPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float specularAmount = 0;
	if (blinn)
	{
		specularAmount = pow(max(dot(normal, halfwayDir), 0.0), uSpecularShininess);
	}
	else
	{
		specularAmount = pow(max(dot(viewDir, reflectLight), 0.0), uSpecularShininess);
	}

	vec4 specularComponent = texture(specularTexture, uvs) * specularAmount * uSpecularColor * uSpotLight.lightColor * uSpotLight.lightIntensity * attenuation;

	vec4 lightTotal = ambientComponent + diffuseComponent + specularComponent;

	return lightTotal;
}

void main()
{

	vec2 uvsEdit = vec2((uvs.x + uOffsetX) * uTileX, (uvs.y + uOffsetY) * uTileY);

	vec4 pointLightResult = PointLightCalc(uvsEdit);

	vec4 directionalLightResult = DirectionalLightCal(uvsEdit);

	vec4 spotlLightResult = SpotLightCal(uvsEdit);

	fragColor = (pointLightResult + directionalLightResult + spotlLightResult) * uColor;
} 
