#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 tbnMatrix;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{

	vec3 T = normalize(vec3(vec4(vsTangents,   0.0)));
	vec3 B = normalize(vec3(vec4(vsBitangents, 0.0)));
	vec3 N = normalize(vec3(vec4(vsNormal,    0.0)));

	T = normalize(T - dot(N, T) * N);
	
	if (dot(cross(N, T), B) < 0.0f){
		 T = T * -1.0f;
	 }

	tbnMatrix = mat3(T, B, N);

	fsPos = vec3(vsPos.x + vsPosition.x, vsPos.y, vsPos.z + vsPosition.y);

	fsNormal = vsNormal;
	fsUv = vsUv;
	gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}