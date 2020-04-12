#version 440 core
layout (location = 0) in vec3 aPos;

out vec3 passColor;

layout(std140, binding = 0) uniform Matrix{
	mat4 model;
	mat4 view;
	mat4 projection;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	passColor = aPos;
}