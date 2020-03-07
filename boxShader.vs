#version 440 core
layout (location = 0) in vec3 aPos;

out vec3 passColor;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
	passColor = aPos;
}