#version 330

uniform mat4 viewProjection;

in vec3 position;
in vec2 texCoord;
out vec2 texCoordV;

in mat4 model;

flat out int instanceId;

void main() {
  
  texCoordV = texCoord;
  instanceId = gl_InstanceID;

  vec4 worldPos = model * vec4(position, 1.0);

  gl_Position = viewProjection * worldPos;
}
