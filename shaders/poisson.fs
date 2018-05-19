#version 330
uniform sampler2D tex;
uniform vec4 color;
in vec2 vsoTC;
out vec4 toto; 

void main() {
  toto = color * texture(tex, vsoTC);
  //toto = vec4(color.rgb + texture(tex, vsoTC).rgb, texture(tex, vsoTC).a); 
}
