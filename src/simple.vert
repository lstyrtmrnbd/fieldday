#version 330

uniform mat4 viewProjection;

in vec3 position;
in vec2 texCoord;
in int texIndex;

in mat4 model;

out vec2 texCoordV;
flat out int texIndexV;
flat out int instanceId;

void main() {
  
  texCoordV = texCoord;
  texIndexV = texIndex;
  instanceId = gl_InstanceID;

  vec4 worldPos = model * vec4(position, 1.0);

  gl_Position = viewProjection * worldPos;
}
