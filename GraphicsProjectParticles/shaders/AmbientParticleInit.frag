#version 400 core

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 velocity;

uniform vec2 uResolution;

// source: http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 seed) {
  return fract(sin(dot(seed.xy,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
  vec2 uv = gl_FragCoord.xy/uResolution.xy;

  position = vec3(uv.x, uv.y, rand(uv));
  velocity = vec3(-2.5, -0.5, 0.0);
}