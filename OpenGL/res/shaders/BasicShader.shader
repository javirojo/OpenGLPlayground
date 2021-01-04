#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 aTextCoord;

out vec3 aColor;
out vec2 uvs;

void main()
{
	gl_Position = position;
	aColor = vertexColor;
	uvs = aTextCoord;
}

#shader fragment
#version 330 core

in vec3 aColor;
in vec2 uvs;
out vec4 fragColor;

uniform vec4 uColor;
uniform sampler2D mainTexture;



void main()
{
	int offsetX = 0;
	int offsetY = 0;
	float tileX = 1f;
	float tileY = 1f;

	/* Texture tiling & offset
	Add value between 0 & 1 in order to offset texture. Default is 0
	Then Multiply in order to tile. Default is 1
	Only works if GL_REPEAT or GL_MIRRORED_REPEAT*/
	vec2 uvsEdit = vec2((uvs.x + offsetX) * tileX, (uvs.y + offsetY) * tileY);	
	fragColor = texture(mainTexture, uvsEdit) ;
	
	
} 
