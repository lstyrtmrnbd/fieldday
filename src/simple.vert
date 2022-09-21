#version 460

in vec3 position;
in vec2 texCoord;
out vec2 texCoordV;

uniform mat4 viewProjection;

void main() {

  texCoordV = texCoord;
  gl_Position = viewProjection * vec4(position, 1.0);
}