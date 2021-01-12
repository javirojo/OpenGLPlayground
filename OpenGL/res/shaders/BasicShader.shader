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

//LIGHT
uniform vec4 uAmbientColor;
uniform float uAmbientIntensity;

uniform vec4 uLightColor;
uniform vec3 uLightPosition;
uniform float uLightIntensity;

uniform vec3 uViewPosition;
uniform vec4 uSpecularColor;
uniform float uSpecularShininess;

uniform bool blinn;

void main()
{
	/* Texture tiling & offset
	Add value between 0 & 1 in order to offset texture. Default is 0
	Then Multiply in order to tile. Default is 1
	Only works if GL_REPEAT or GL_MIRRORED_REPEAT*/
	vec2 uvsEdit = vec2((uvs.x + uOffsetX) * uTileX, (uvs.y + uOffsetY) * uTileY);

	//Light contribution (Phong) = ambient + diffuse + specular
	vec4 ambientComponent = texture(diffuseTexture, uvsEdit) *  uAmbientColor * uAmbientIntensity;

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(uLightPosition - fragPos);
	float diffuseAmount = max(dot(norm, lightDir), 0.0);
	vec4 diffuseComponent = texture(diffuseTexture, uvsEdit) * diffuseAmount * uLightColor * uLightIntensity;

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
	
	vec4 specularComponent = texture(specularTexture, uvsEdit) * specularAmount  * uSpecularColor * uLightColor * uLightIntensity;

	vec4 lightTotal = ambientComponent + diffuseComponent + specularComponent;

	
	fragColor = lightTotal * uColor;
	
	
} 
