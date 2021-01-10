#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 aTextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uvs;

void main()
{
	gl_Position = projection * view * model * position;
	uvs = aTextCoord;
}

#shader fragment
#version 330 core

in vec2 uvs;
out vec4 fragColor;

uniform vec4 uColor;
uniform sampler2D mainTexture;
uniform float uOffsetX;
uniform float uOffsetY;
uniform float uTileX;
uniform float uTileY;


void main()
{
	/* Texture tiling & offset
	Add value between 0 & 1 in order to offset texture. Default is 0
	Then Multiply in order to tile. Default is 1
	Only works if GL_REPEAT or GL_MIRRORED_REPEAT*/
	vec2 uvsEdit = vec2((uvs.x + uOffsetX) * uTileX, (uvs.y + uOffsetY) * uTileY);
	fragColor = texture(mainTexture, uvsEdit) * uColor;
	
	
} 
