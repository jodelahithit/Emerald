#version 400 core

in vec2 position;

out vec2 textureCoords;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = transformationMatrix * vec4(position, 0, 1);
    textureCoords = vec2((position.x+1.0)/2.0, (position.y+1.0)/2.0);
    //textureCoords += texPos;
    //textureCoords /= texResolution;
    //textureCoords *= texSize;
}