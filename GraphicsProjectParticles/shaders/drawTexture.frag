#version 400 core

in vec2 inUV;

out vec4 outColour;

uniform sampler2D uTexture;

void main() {
	outColour = texture( uTexture, inUV );
}