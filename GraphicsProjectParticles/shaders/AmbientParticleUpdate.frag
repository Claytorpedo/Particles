#version 400 core

#define GRAV_OBJS 10

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 velocity;

#define K_DECEL 0.99
#define EPS 0.00001 // Keeps us from division by zero.

uniform vec2 uResolution;
uniform float uElapsedTime;
uniform int uCohesiveness; // Keeps the particles together. Higher number = less together.
uniform vec4 uGravity[GRAV_OBJS];
uniform uint uActiveGrav[GRAV_OBJS];
uniform sampler2D uTexPos;
uniform sampler2D uTexVel; 

void main() {
  vec2 uv = gl_FragCoord.xy/uResolution.xy;

  vec3 pos = texture(uTexPos, uv).rgb;
  vec3 vel = texture(uTexVel, uv).rgb;

  // Update position.
  position = pos + vel * uElapsedTime;

  // Update velocity.

  vel = vel * K_DECEL;

  for (int i = 0; i < GRAV_OBJS; ++i) {
      if ( uActiveGrav[i] != 0 ) {
		  vec4 grav = uGravity[i];
          vec3 toGrav = grav.xyz - pos;
          float d2 = toGrav.x * toGrav.x + toGrav.y * toGrav.y + toGrav.z * toGrav.z;
          vec3 accel = toGrav * grav.w / clamp((d2 == 0.0 ? EPS : d2), -uCohesiveness, uCohesiveness);
		  vel += accel * uElapsedTime;
	  }
  }

  velocity = vel;
}