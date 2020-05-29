#version 330

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec3 vsIndex; //x = vertex; y = block side; z = texture;

out vec2 fsUv;
out vec3 fsNormal;
out vec3 fsTangent;

uniform vec3 _ChunkPos;

struct Data {
	vec3 uv;
	mat3 TBNMatrix;
};

out Data fsData;

vec2 texCoords[4] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);

vec3 normals[6] = vec3[6](
	vec3( -1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 0, 0, 1 ),
	vec3( 0, 0, -1 ),
	vec3( 0, 1, 0 ),
	vec3( 0, -1, 0 )
);

vec3 tangents[6] = vec3[6](
	vec3( 0, 0, -1 ),
	vec3( 0, 0, -1 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 )
);

void main() {  
    vec3 pos = vsPos + _ChunkPos;
    gl_Position = _Projection * _View * vec4(pos, 1.0);
    vec3 n = normals[int(vsIndex.y)];
    vec3 t = tangents[int(vsIndex.y)];
	vec3 b = cross(n, t);
	fsData.TBNMatrix = mat3(t, b, n);

    fsData.uv = vec3(texCoords[int(vsIndex.x)], vsIndex.z);
}