#version 400 core

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 velocity;
layout(location = 2) out vec4 other;

#define K_VEL_DECAY 0.99

uniform vec2 uResolution;
uniform float uDeltaT;
uniform vec3 uInputPos;
uniform float uKForce;
uniform sampler2D uTexPos;  // pos
uniform sampler2D uTexVel;  // vel
uniform sampler2D uTexOther;  // unused here.

void main() {
  vec2 uv = gl_FragCoord.xy/uResolution.xy;

  // read data
  vec3 pos = texture(uTexPos, uv).rgb;
  vec3 vel = texture(uTexVel, uv).rgb;

  // compute force
  vec3 toCenter = uInputPos - pos;
  float toCenterLength = length(toCenter);
  vec3 accel = (toCenter/toCenterLength) * uKForce / toCenterLength;

  // update particle
  // important, order matters
  pos += vel * uDeltaT;
  vel = K_VEL_DECAY * vel + accel * uDeltaT;

  // write out data
  position = vec4(pos, 1.0);
  velocity = vec4(vel, 1.0);
  other = texture(uTexOther, uv);
}