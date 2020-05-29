#version 330

#include "includes/globalUniforms.incl"

in vec2 fsUV;

out vec3 outColor;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _SSAO;

uniform float _AmbientIntensity;

void main(){
	vec4 misc = texture(_GMisc, fsUV);
	vec4 albedo = texture(_GAlbedo, fsUV);
	vec3 emission = vec3(misc.z, misc.w, albedo.w) * vec3(2.0);
	float ssao = texture(_SSAO, fsUV).x;

	outColor = (albedo.xyz * _AmbientIntensity * ssao) + emission;
}