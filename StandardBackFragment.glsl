#version 440 core
in vec3 passColor;
out vec4 FragColor;
void main()
{
    
	FragColor = vec4(passColor,1); // ���������ĸ�����ȫ������Ϊ1.0
}