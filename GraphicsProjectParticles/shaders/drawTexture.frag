#version 400 core

in vec2 aUV;

out vec4 outColour;

uniform sampler2D uTexture;

void main() {
	outColour = texture( uTexture, aUV );
}