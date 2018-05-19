/*!\file animations.h
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 05, 2014
 */
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif


  extern void animation_vide(int state);
  extern void exemple_de_transition_00(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void exemple_de_transition_01(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void exemple_d_animation_00(int state);
  extern void exemple_d_animation_01(int state);
  extern void exemple_d_animation_02(int state);
  extern void exemple_d_animation_03(int state);
  extern void exemple_d_animation_04(int state);
  extern void exemple_d_animation_05(int state);
  extern void exemple_d_animation_06(int state);
  extern void animationsInit(void);
  extern void base(int state);
  extern void shark(int state);
  extern void initShark(void);
  extern void drawShark(void);
  extern int random_range (int min, int max);
  //extern void initBalle(int n);
  //extern void drawBalle(void);
  extern void balle(int state);
  
  extern void poisson(int state);
  /* earth.c */
  extern void earth(int state);
  extern void credit(int state);
#ifdef __cplusplus
}
#endif

#endif
