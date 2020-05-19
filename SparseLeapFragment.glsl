#version 440 core

in vec3 EntryPoint;
in vec3 texCoord;

uniform sampler2D exitPoints;
uniform sampler3D volume;
uniform sampler1D transferFunc;
uniform float StepSize = 0.001f;
uniform vec3 viewPoint;
uniform vec2 ScreenSize;
layout(binding = 0, r32ui) uniform uimage2D CountTexture2D;
layout(binding = 1, r32f) uniform image3D DepthTexture3D;
layout(binding = 2, r32ui) uniform uimage3D ClassTexture3D;
layout(binding = 3, r32ui) uniform uimage3D TypeTexture3D;
layout(binding = 4, rgba32f) uniform image2D FrontPositionTexture2D;
	
layout(location = 0) out vec4 FragColor;

uint currentLength = 0;
uint listLength = 0;
vec3 frontPos;             
vec3 dir;
vec3 deltaDir;
float deltaDirLen;
vec4 colorAcum = vec4(0.0);
float alphaAcum = 0.0;
float intensity;
vec4 colorSample;
float alphaSample;
vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.0);

struct Event {
	float depth;
	uint eventType;
	uint eventClass;
};

Event GetNextRayEvent() {
	Event event;
	event.depth = imageLoad(DepthTexture3D, ivec3(gl_FragCoord.xy, currentLength)).x;
	event.eventType = imageLoad(TypeTexture3D, ivec3(gl_FragCoord.xy, currentLength)).x;
	event.eventClass = imageLoad(ClassTexture3D, ivec3(gl_FragCoord.xy, currentLength)).x;
	currentLength++;
	return event;
}

bool ListIsEmpty() {
	if (currentLength < listLength) return false;
	else return true;
}

bool Sample(Event eventSegBegin, Event eventSegEnd) {
	vec3 beginPos = texCoord + normalize(dir) * eventSegBegin.depth;
	vec3 voxelCoord = beginPos;
	float DepthAcum = eventSegBegin.depth;
	bool flag = false;
	for (int i = 0; i < 1600; i++) {
		intensity = texture(volume, voxelCoord).x;	// 获得体数据中的标量 
		colorSample = texture(transferFunc, intensity);	//通过传递函数进行查询
		voxelCoord += deltaDir;
		DepthAcum += deltaDirLen;

		if (colorSample.a > 0.0) {
			if (gl_FragCoord.x < 640) {
				colorSample.a = 1.0 - pow(1.0 - colorSample.a, StepSize * 200.0f);
				colorAcum.rgb += (1.0 - colorAcum.a) * colorSample.rgb * colorSample.a;
				colorAcum.a += (1.0 - colorAcum.a) * colorSample.a;
			}
			else {
				colorSample.a = 1 - colorSample.a;
				colorAcum.rgb += colorSample.rgb * colorAcum.a;
				colorAcum.a = (colorAcum.a) * colorSample.a;
			}
			
			
		}
		if (DepthAcum >= eventSegEnd.depth) {	//超出光线范围
			//colorAcum.rgb = colorAcum.rgb * colorAcum.a + (1 - colorAcum.a) * bgColor.rgb;
			break;
		}
		else if (colorAcum.a > 1.0) {
			colorAcum.a = 1.0;
			flag = true;
			break;
		}
	}
	return flag;
}

void main() {
	vec3 exitPoint = texture(exitPoints, gl_FragCoord.st / ScreenSize).xyz;
	dir = exitPoint - EntryPoint;
	deltaDir = normalize(dir) * StepSize;
	deltaDirLen = length(deltaDir);
	listLength = imageLoad(CountTexture2D, ivec2(gl_FragCoord.xy)).x;
	int i = 1;
	Event eventSegBegin = GetNextRayEvent();
	Event eventSegEnd;
	while (!ListIsEmpty()) {
		eventSegEnd = GetNextRayEvent();
		if (eventSegBegin.eventClass != 0) {
			bool flag = Sample(eventSegBegin, eventSegEnd);
			if (flag) break;
			if (eventSegBegin.eventClass == 2) {
				//todo ReportUnknown
			}
		}
		eventSegBegin = eventSegEnd;
		i++;
	}
	//colorAcum.rgb = colorAcum.rgb * colorAcum.a + (1 - colorAcum.a) * bgColor.rgb;
	if (colorAcum.a != 1.0) {
		colorAcum.rgb = colorAcum.rgb * colorAcum.a + (1 - colorAcum.a) * bgColor.rgb;
	}
	FragColor = colorAcum;
	imageStore(FrontPositionTexture2D, ivec2(gl_FragCoord.xy), vec4(float(i)));
	float a = float(i) / 30;
	//FragColor = vec4(vec3(a),1);
}
