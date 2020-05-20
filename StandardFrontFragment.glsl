#version 440 core

in vec3 EntryPoint;
in vec4 ExitPointCoord;
in vec3 texCoord;

uniform sampler2D exitPoints;
uniform sampler3D volume;
uniform sampler1D transferFunc;  
uniform float     StepSize = 0.001f;
uniform vec2      ScreenSize;
layout (location = 0) out vec4 FragColor;

void main(){
    vec3 exitPoint = texture(exitPoints, gl_FragCoord.st/ScreenSize).xyz;

    if (EntryPoint == exitPoint)
    	discard;	//无需对背景进行处理
    vec3 dir = exitPoint - EntryPoint;
    float len = length(dir);	// 包围盒前后点之间光线的长度
    vec3 deltaDir = normalize(dir) * StepSize;
    float deltaDirLen = length(deltaDir);
    vec3 voxelCoord = texCoord;
    vec4 colorAcum = vec4(0.0);		
    float alphaAcum = 0.0;           
    float intensity;
    float lengthAcum = 0.0;
    vec4 colorSample; 
    float alphaSample; 
    vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.0);
	
	vec4 temp = texture(volume, voxelCoord);
    for(int i = 0; i < 2000; i++){
    	intensity =  texture(volume, voxelCoord).x;	// 获得体数据中的标量 
    	colorSample = texture(transferFunc, intensity);	//通过传递函数进行查询

    	if (colorSample.a > 0.0) {
    	    //colorSample.a = 1.0 - pow(1.0 - colorSample.a, StepSize*200.0f);
    	    colorAcum.rgb += (1.0 - colorAcum.a) * colorSample.rgb * colorSample.a;
    	    colorAcum.a += (1.0 - colorAcum.a) * colorSample.a;
    	}
    	voxelCoord += deltaDir;
    	lengthAcum += deltaDirLen;
    	if (lengthAcum >= len ) {	//超出光线范围
    	    colorAcum.rgb = colorAcum.rgb*colorAcum.a + (1 - colorAcum.a)*bgColor.rgb;		
    	    break;	
    	}	
    	else if (colorAcum.a > 1.0) {//不透明度达到1
    	    colorAcum.a = 1.0;
    	    break;
    	}
    }
    FragColor = colorAcum;
}
