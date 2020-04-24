#version 440 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 texacoord;
layout(location = 2) in vec3 offset;
layout(location = 3) in float scale;
layout(location = 4) in uint aFaceOrder;
layout(location = 5) in uint aNodeClass;
layout(location = 6) in uint aParentClass;
 
layout(std140, binding = 0) uniform Matrix{
	mat4 model;
	mat4 view;
	mat4 projection;
};

flat out uint faceOrder;
flat out uint nodeClass;
flat out uint parentClass;
out vec3 position;
out vec3 color;

void main()
{
	gl_Position = projection * view *  model * vec4( (aPos * scale + offset), 1);
	faceOrder = aFaceOrder;
	nodeClass = aNodeClass;
	parentClass = aParentClass;
	position = vec3(model * vec4((aPos * scale + offset), 1));
	color = texacoord;
}