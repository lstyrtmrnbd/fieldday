#version 330

// in vec2 texCoordV;
out vec4 fragColor;

flat in int instanceId;

void main() {

  fragColor = vec4(0.3, 0.9, 0.6, 1.0);
}
