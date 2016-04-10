#version 400 core

in vec2 inUV;

uniform float uPointSize;
uniform mat4 uPVM;
uniform vec4 uColour;
uniform sampler2D uTexPos;
uniform sampler2D uTexVel;
uniform sampler2D uTexOther; // Could be used for point sprites.

void main() {
  gl_PointSize = uPointSize;

  gl_Position = uPVM * texture(uTexPos, inUV);
}