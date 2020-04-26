#version 440 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 atexCoord;

layout(std140, binding = 0) uniform Matrix{
	mat4 model;
	mat4 view;
	mat4 projection;
};

out vec3 EntryPoint;
out vec3 texCoord;

void main()
{	
	EntryPoint = aPos;
	texCoord = atexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1);
}