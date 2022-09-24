#version 330

uniform int multiplicationFactor = 8;
uniform float threshold = 0.133;

uniform usampler2DArray texs;

in vec2 texCoordV;
flat in int instanceId;
flat in int texIndexV;

out vec4 fragColor;

void main() {

  float r = 0.3 * instanceId;

  // // multiplicationFactor scales the number of stripes
  // vec2 t = texCoordV * multiplicationFactor;

  // // // the threshold constant defines the width of the lines
  // if (fract(t.s) < threshold  || fract(t.t) < threshold )
  //   fragColor = vec4(0.0, 0.0, 1.0, 1.0);	
  // else
  //   discard;

  // fragColor = vec4(0.0, texCoordV.t, texCoordV.s, 1.0);

  fragColor = texture(texs, vec3(texCoordV, texIndexV));
}
