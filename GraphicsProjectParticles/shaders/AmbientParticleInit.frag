#version 400 core

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 velocity;

uniform vec2 uResolution;

// source: http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 seed) {
  return fract(sin(dot(seed.xy,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
  vec2 uv = gl_FragCoord.xy/uResolution.xy;

  vec3 pos = vec3(uv.x, uv.y, rand(uv));
  vec3 vel = vec3(-2.0, 0.0, 0.0);

  position = vec4(pos, 1.0);
  velocity = vec4(vel, 1.0);
}