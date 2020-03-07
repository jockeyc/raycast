#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 texacoord;

uniform mat4 mvp;

out vec3 EntryPoint;
out vec3 texCoord;
out vec4 ExitPointCoord;

void main()
{
    gl_Position = mvp*vec4(aPos,1);
	EntryPoint = aPos;
	ExitPointCoord = gl_Position;
	texCoord = texacoord;
}