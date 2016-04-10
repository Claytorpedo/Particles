#version 400 core

layout(location = 0) in vec3 inVertexPos;
//in vec3 inVertexColour;

// Uniform values stay constant for the whole mesh (across every vertex).
uniform mat4 uPVM;

out vec3 outColour;

void main() {
	outColour = vec3(1,0,0);//inVertexColour;
	gl_Position = uPVM * vec4(inVertexPos, 1.0);
}