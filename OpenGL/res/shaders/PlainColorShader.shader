#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * position;
}

#shader fragment
#version 330 core

out vec4 fragColor;

uniform vec4 uColor;

void main()
{	
	fragColor = uColor;
}
