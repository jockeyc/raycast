#version 440 core
#extension GL_ARB_fragment_shader_interlock : require

layout(pixel_interlock_ordered) in;

flat in uint faceOrder;
flat in uint nodeClass;
flat in uint parentClass;
flat in int id;
in vec3 position;
in vec3 color;

layout(binding = 0, r32ui) uniform uimage2D CountTexture2D;
layout(binding = 1, r32f) uniform image3D DepthTexture3D;
layout(binding = 2, r32ui) uniform uimage3D ClassTexture3D;
layout(binding = 3, r32ui) uniform uimage3D TypeTexture3D;
layout(binding = 4, rgba32f) uniform image2D FrontPositionTexture2D;

out vec4 FragColor;

uniform vec3 cameraPos;

struct Event {
	float depth;
	uint eventType;
	uint eventClass;
};
uint Length;
float closeDistance = 0.1f;

void GetRaySegmentList() {
	Length = imageLoad(CountTexture2D, ivec2(gl_FragCoord.xy)).x;
}

Event getPrevEvent() {
	Event event;
	event.depth = imageLoad(DepthTexture3D, ivec3(gl_FragCoord.xy, Length - 1)).x;
	event.eventType = imageLoad(TypeTexture3D, ivec3(gl_FragCoord.xy, Length - 1)).x;
	event.eventType = imageLoad(ClassTexture3D, ivec3(gl_FragCoord.xy, Length - 1)).x;
	return event;
}

Event getPrevPrevEvent() {
	Event event;
	event.depth = imageLoad(DepthTexture3D, ivec3(gl_FragCoord.xy, Length - 2)).x;
	event.eventType = imageLoad(TypeTexture3D, ivec3(gl_FragCoord.xy, Length - 2)).x;
	event.eventType = imageLoad(ClassTexture3D, ivec3(gl_FragCoord.xy, Length - 2)).x;
	return event;
}

void overwritePrevEvent(float depth, uint eventType, uint eventClass) {
	imageStore(DepthTexture3D, ivec3(gl_FragCoord.xy, Length - 1), vec4(depth));
	imageStore(TypeTexture3D, ivec3(gl_FragCoord.xy, Length - 1), uvec4(eventType));
	imageStore(ClassTexture3D, ivec3(gl_FragCoord.xy, Length - 1), uvec4(eventClass));
}

void deletePrevEvent() {
	Length--;
	imageStore(CountTexture2D, ivec2(gl_FragCoord.xy), uvec4(Length));
}

void appendEvent(float depth, uint eventType, uint eventClass) {
	Length++;
	imageStore(CountTexture2D, ivec2(gl_FragCoord.xy), uvec4(Length));
	imageStore(DepthTexture3D, ivec3(gl_FragCoord.xy, Length - 1), vec4(depth));
	imageStore(TypeTexture3D, ivec3(gl_FragCoord.xy, Length - 1), uvec4(eventType));
	imageStore(ClassTexture3D, ivec3(gl_FragCoord.xy, Length - 1), uvec4(eventClass));
}

bool isClose(float depth1, float depth2) {
	if (abs(depth1 - depth2) <= closeDistance)return true;
	else return false;
}

bool MergeRayEvents(float depth, uint eventType, uint eventClass)  {
	bool flag = false;
	Event eventPrev;
	Event eventPrevPrev;
	if (Length == 0){
		return flag;
	}
	else if (Length == 1) {
		eventPrev = getPrevEvent();
		if (eventPrev.depth == depth) {
			flag = true;
			if (eventPrev.eventType == eventType) {
				overwritePrevEvent(depth, eventType, eventClass);
			}
			else if (eventType == 1) {
				deletePrevEvent();
			}
		}
	}
	else {
		eventPrev = getPrevEvent();
		eventPrevPrev = getPrevPrevEvent();
		if (eventPrev.depth == depth) {
			flag = true;
			if (eventPrev.eventType == eventType) {
				overwritePrevEvent(depth, eventType, eventClass);
			}
			else if (eventType == 1) {
				deletePrevEvent();
			}
			else if (eventPrevPrev.eventClass == eventClass) {
				deletePrevEvent();
			}
		}
	}
	if (isClose(eventPrev.depth, depth) && eventPrev.eventClass == 0) {
		if (eventClass == 1) {
			if (eventPrev.eventType == eventType) {
				overwritePrevEvent(eventPrev.depth, eventType, eventClass);
				flag = true;
			}
			else {
				deletePrevEvent();
				flag = true;
			}
		}
		else if (eventClass == 2) {
			overwritePrevEvent(depth, eventType, eventClass);
			flag = true;
		}
	}
	return flag;
}

void AddRayEvent(float depth, uint eventType, uint eventClass) {
	bool hasDone = MergeRayEvents(depth, eventType, eventClass);
	if (!hasDone) appendEvent(depth, eventType, eventClass);
}

void main()
{
	beginInvocationInterlockARB();
	bool isFrontFace = (faceOrder == 0);
	
	if ((!gl_FrontFacing && isFrontFace) || (gl_FrontFacing && !isFrontFace)) {
		discard;
	}
	GetRaySegmentList();
	if (parentClass == 3 && gl_FrontFacing) {
		imageStore(FrontPositionTexture2D, ivec2(gl_FragCoord.xy), vec4(position, 0.0f));
	}
	//if (parentClass == 3) discard;
	vec3 frontPos = imageLoad(FrontPositionTexture2D, ivec2(gl_FragCoord.xy)).xyz;
	float dist = length(position-frontPos);
	if (isFrontFace) {
		AddRayEvent(dist, faceOrder, nodeClass);
	}
	else {
		AddRayEvent(dist, faceOrder, parentClass);
	}
	float a = float(id);
	a = a / 23574f*3;
	FragColor = vec4(vec3(a), 1);
	endInvocationInterlockARB();
}