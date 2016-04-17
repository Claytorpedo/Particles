#version 400 core

in vec3 inColour;

//out vec4 outColour;

void main() {
	//outColour = vec4(1,0,0, 1.0);
	gl_FragData[0] = vec4(1,0,0, 1.0);
}