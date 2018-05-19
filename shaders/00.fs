/*!\file basic.fs
 *
 * \brief rendu avec lumière directionnelle diffuse et couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date April 15 2016
 */
#version 330
uniform mat4 modelViewMatrix;
uniform vec4 couleur;
uniform sampler2D tex;
uniform int notex;

in  vec3 vsoNormal;
in  vec3 vsoModPos;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  vec3 N = normalize(vsoNormal);
  vec3 L = normalize(vec3(-1, -1, 0)); /*vers le bas vers la gauche*/
  float diffuse = clamp(dot(N, -L), 0, 1);
  if(notex == 1)
    fragColor = vec4((couleur.rgb * diffuse), couleur.a);
  else
    fragColor = vec4(texture(tex, 10 * vsoTexCoord).rgb * (couleur.rgb * diffuse), couleur.a);
}
