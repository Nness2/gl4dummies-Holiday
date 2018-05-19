#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 coordTex;
uniform mat4 mod;
out vec2 vsoTC;

void main() {
  gl_Position = mod * vec4(position, 1);
  vsoTC = coordTex;
}
