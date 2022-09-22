#version 330

// I don't see how you can hate from outside of the club..

// you can't even get in

uniform mat4 viewProjection;

in vec3 position;
in vec2 texCoord;
out vec2 texCoordV;

in mat4 models[3];

flat out int instanceId;

void main() {
  
  texCoordV = texCoord;
  instanceId = gl_InstanceID;

  mat4 model = models[instanceId];

  vec4 worldPos = vec4(position, 1.0) * model;

  gl_Position = worldPos * viewProjection;
  // gl_Position = position;
}
