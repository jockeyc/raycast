#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 texacoord;

layout(std140, binding = 0) uniform Matrix{
	mat4 model;
	mat4 view;
	mat4 projection;
};

out vec3 EntryPoint;
out vec3 texCoord;
out vec4 ExitPointCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos,1);
	EntryPoint = aPos;
	ExitPointCoord = gl_Position;
	texCoord = texacoord;
}