#version 400 core

uniform vec4 uColour;

void main() {
  // Draw circular points by discarding fragments on the square's edges.
  vec2 circleCoord = 2.0 * gl_PointCoord - 1.0;
  if ( dot(circleCoord, circleCoord) > 1.0 ) {
	discard;
  }
  gl_FragColor = uColour;
}