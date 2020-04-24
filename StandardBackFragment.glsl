#version 440 core
in vec3 passColor;
out vec4 FragColor;
void main()
{
    
	FragColor = vec4(passColor,1); // 将向量的四个分量全部设置为1.0
}