#version 400 core

layout(location = 0) in vec3 inVertexPos;

out vec2 outUV;

void main() {
	gl_Position = vec4(inVertexPos, 1.0);
	outUV = ( inVertexPos.xy + vec2(1,1) ) / 2.0;
}