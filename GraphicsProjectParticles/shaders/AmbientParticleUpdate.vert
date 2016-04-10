#version 400 core

in vec3 inVertexPos;

void main() {
	gl_Position = vec4(inVertexPos, 1.0);
}