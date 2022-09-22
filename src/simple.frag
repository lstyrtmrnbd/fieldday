#version 330

in vec2 texCoordV;
out vec4 fragColor;

flat in int instanceId;

void main() {

  float r = 0.3 * instanceId;
  
  fragColor = vec4(r, 0.9, 0.6, 1.0);
}
