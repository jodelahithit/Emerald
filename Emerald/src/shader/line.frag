#version 400

in vec3 fsPos;
out vec4 geoData[4];

void main(){
	geoData[0] = vec4(0, 1, 0, 0);
	geoData[1] = vec4(200, 0, 0, 1);
	//geoData[2] = vec4(0, 0, 0, 1);
	//geoData[3] = vec4(0, 0, 0, ); Ignore position for SSAO and stuff
}