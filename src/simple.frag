#version 330

uniform int multiplicationFactor = 8;
uniform float threshold = 0.1;

in vec2 texCoordV;
out vec4 fragColor;

flat in int instanceId;

void main() {

  float r = 0.3 * instanceId;

  // multiplicationFactor scales the number of stripes
  // vec2 t = texCoordV * multiplicationFactor;

  // // the threshold constant defines the width of the lines
  // if (fract(t.s) < threshold  || fract(t.t) < threshold )
  //   fragColor = vec4(r, 0.0, 1.0, 1.0);	
  // else
  //   discard;

  fragColor = vec4(r, texCoordV, 1.0);
}
