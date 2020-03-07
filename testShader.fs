#version 440 core
out vec4 FragColor;

in vec3 entryPoint;

uniform sampler2D exitPoints;
uniform sampler3D volume;
vec2 ScreenSize = vec2(800,600);
void main()
{
    vec3 exitPoint = texture(exitPoints, gl_FragCoord.st/ScreenSize).xyz;
	if(entryPoint == exitPoint)
		discard;
	vec3 dir = exitPoint - entryPoint;
    float len = length(dir); // the length from front to back is calculated and used to terminate the ray
	float StepSize = 0.001f;
    vec3 deltaDir = normalize(dir) * StepSize;
    float deltaDirLen = length(deltaDir);
    vec3 voxelCoord = entryPoint;
    vec4 colorAcum = vec4(0.0); // The dest color
    float alphaAcum = 0.0;                // The  dest alpha for blending
	float intensity;
    float lengthAcum = 0.0;
    vec4 colorSample; // The src color 
    float alphaSample; // The src alpha
	vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.0);

    FragColor = vec4(vec3(exitPoint.z),1);
}