/*!\file animations.c
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

#include <GL4D/gl4dh.h>
#include "audioHelper.h"
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>

/*!\brief identifiant de la géométrie QUAD GL4Dummies */
static GLuint _quadId = 0;

void animation_vide(int state) {
  switch(state) {
  case GL4DH_DRAW:
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
  default:
    return;
  }
}

void exemple_de_transition_00(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER LES DEUX ANIMATIONS */
    if(a0) a0(state);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    return;
  }
}

void exemple_de_transition_01(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[2], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(2, tex);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mix.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(2, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

#define ECHANTILLONS 1024
void exemple_d_animation_00(int state) {
  static Sint16 _hauteurs[ECHANTILLONS];
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  static GLuint _screen = 0;
  Sint16 * s;
  static GLfloat c[4] = {0, 0, 0, 0};
  switch(state) {
  case GL4DH_INIT:
    _screen = gl4dpInitScreen();
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    if(l >= 2 * ECHANTILLONS)
      for(i = 0; i < ECHANTILLONS; i++)
	_hauteurs[i] = gl4dpGetHeight() / 2 + (gl4dpGetHeight() / 2) * s[i] / ((1 << 15) - 1.0);
    return;
  default: /* GL4DH_DRAW */
    gl4dpSetScreen(_screen);
    gl4dpClearScreen();
    for(i = 0; i < ECHANTILLONS - 1; i++) {
      int x0, y0, x1, y1;
      x0 = (i * (gl4dpGetWidth() - 1)) / (ECHANTILLONS - 1);
      y0 = _hauteurs[i];
      x1 = ((i + 1) * (gl4dpGetWidth() - 1)) / (ECHANTILLONS - 1);
      y1 = _hauteurs[i + 1];
      gl4dpSetColor(rand());
      gl4dpLine(x0, y0, x1, y1);
    }
    gl4dpUpdateScreen(NULL);

    /* JOUER L'ANIMATION */
/*     glClearColor(c[0], c[1], c[2], c[3]); */
/*     glClear(GL_COLOR_BUFFER_BIT); */
    return;
  }
}

void exemple_d_animation_01(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glDisable(GL_DEPTH);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}

void exemple_d_animation_02(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLuint tId = 0, pId;
  static const GLubyte t[16] = {-1,  0,  0, -1, /* rouge */
				 0, -1,  0, -1, /* vert  */
				 0,  0, -1, -1, /* bleu  */
				-1, -1,  0, -1  /* jaune */ };
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glGenTextures(1, &tId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tId) {
      glDeleteTextures(1, &tId);
      tId = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glDisable(GL_DEPTH);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(pId);
    glBindTexture(GL_TEXTURE_2D, tId);
    glUniform1i(glGetUniformLocation(pId, "myTexture"), 0);
    gl4dgDraw(_quadId);
    return;
  }
}

void exemple_d_animation_03(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint rect[4], tr, mr;
  static int r = 0, or = 0;
  static GLuint screen_id;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    screen_id = gl4dpInitScreen();
    gl4dpUpdateScreen(NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 6);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    mr = MAX(tr, or) + 1;
    gl4dpSetScreen(screen_id);
    rect[0] = MAX((gl4dpGetWidth()  >> 1) - mr, 0);
    rect[1] = MAX((gl4dpGetHeight() >> 1) - mr, 0);
    rect[2] = MIN(mr << 1,  gl4dpGetWidth() - rect[0]);
    rect[3] = MIN(mr << 1, gl4dpGetHeight() - rect[1]);
    gl4dpSetColor(RGB(0, 0, 0));
    gl4dpRect(rect);
    gl4dpSetColor(RGB(255, 255, 255));
    gl4dpFilledCircle(gl4dpGetWidth() >> 1, gl4dpGetHeight() >> 1, tr);
    gl4dpUpdateScreen(rect);
    or = tr;
    return;
  }
}

void exemple_d_animation_04(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int l, i;
  Sint16 * s;
  GLint tr;
  static GLint r = 0;
  static GLuint s1, s2;
  static GLfloat r1[] = {0.0, 0.0, 1.0, 1.0}, r2[] = {0, 0, 1, 1}, rf = 0.0;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    s1 = gl4dpInitScreen();
    s2 = gl4dpInitScreen();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(s1);
    gl4dpDeleteScreen();
    gl4dpSetScreen(s2);
    gl4dpDeleteScreen();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Sint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0, tr = 0; i < l >> 1; i++)
      tr += abs(s[i]);
    tr /= l >> 1;
    r = 100 + (tr >> 5);
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    tr = r;
    gl4dpSetScreen(s2);
    gl4dpClearScreen();
    for(i = 0; i < tr; i++) {
      Uint8 r = rand()&255, g = rand()&255, b = rand()&255;
      gl4dpSetColor(RGB(r, g, b));
      gl4dpCircle(gl4dpGetWidth() >> 2, gl4dpGetHeight() >> 1, i);
    }
    gl4dpSetColor(RGB(255, 255, 255));
    gl4dpFilledCircle(gl4dpGetWidth() * 3 / 4, gl4dpGetHeight() >> 1, i);
    gl4dpUpdateScreen(NULL);
    rf += 0.05;
    gl4dpSetScreen(s1);
    gl4dpClearScreen();
    gl4dpMap(s1, s2, r1, r2, rf);
    gl4dpUpdateScreen(NULL);
    return;
  }
}

/* exemple simpliste de recopie depuis une surface sdl vers le screen en cours */
static void copyFromSurface(SDL_Surface * s, int x0, int y0) {
  Uint32 * p, coul, ocoul;
  Uint8 rmix, gmix, bmix;
  double f1, f2;
  int x, y, maxx, maxy;
  assert(s->format->BytesPerPixel == 4); /* pour simplifier, on ne gère que le format RGBA */
  p = s->pixels;
  maxx = MIN(x0 + s->w, gl4dpGetWidth());
  maxy = MIN(y0 + s->h, gl4dpGetHeight());
  for(y = y0; y < maxy; y++) {
    for(x = x0; x < maxx; x++) {
      ocoul = gl4dpGetPixel(x, y);
      coul = p[(s->h - 1 - (y - y0)) * s->w + x - x0]; /* axe y à l'envers */
      f1 = ALPHA(coul) / 255.0; f2 = 1.0 - f1;
      rmix = f1 *   RED(coul) + f2 *   RED(ocoul);
      gmix = f1 * GREEN(coul) + f2 * GREEN(ocoul);
      bmix = f1 *  BLUE(coul) + f2 *  BLUE(ocoul);
      gl4dpSetColor(RGB(rmix, gmix, bmix));
      gl4dpPutPixel(x, y);
    }
  }
}

int random_range (int min, int max)
{
   return min + ((max - min) * (rand () / (double) RAND_MAX));
}

int inc = 0;
void exemple_d_animation_05(int state) {
  /* INITIALISEZ VOS VARIABLES */
  int i, nbr_image;
  nbr_image = 5;
  static char * image[5] = {"mer.jpg","sable.png","serviette.png","parasol.png","ballon.png"};

  static SDL_Surface * sprite[5];
  // for (i = 0 ; i < nbr_image ; i++){
  //   sprite[i] == NULL;
  // }
  static GLuint screen_id = 0;
  switch(state) {
  case GL4DH_INIT: {
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    SDL_Surface * s[nbr_image];
    for (i = 0 ; i < nbr_image ; i++){
      s[i] = IMG_Load(image[i]);
      sprite[i] = SDL_ConvertSurfaceFormat(s[i], SDL_PIXELFORMAT_ABGR8888, 0);
      SDL_FreeSurface(s[i]);
    }
    screen_id = gl4dpInitScreen();
    gl4dpClearScreenWith(0xFFFFFFFF);
    return;
  }
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(screen_id);
    gl4dpDeleteScreen();
    for (i = 0 ; i < nbr_image ; i++){
      if(sprite[i]) {
        SDL_FreeSurface(sprite[i]);
        sprite[i] = NULL;
      }
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    gl4dpSetScreen(screen_id);
    inc++;
    if (inc < 200)
      copyFromSurface(sprite[0], rand() % gl4dpGetWidth(), rand() % gl4dpGetHeight());
    if (inc > 200 && inc < 300 && inc % 1 == 0)
      copyFromSurface(sprite[1], rand() % gl4dpGetWidth(), (rand() % gl4dpGetHeight())/4);
    if (inc > 300 && inc < 350 && inc % 7 == 0)
      copyFromSurface(sprite[2], rand() % gl4dpGetWidth(), (rand() % gl4dpGetHeight())/3);
    if (inc > 350 && inc < 400 && inc % 7 == 0)
      copyFromSurface(sprite[4], rand() % gl4dpGetWidth(), (rand() % gl4dpGetHeight())/2);
    if (inc > 400 && inc < 455 && inc % 7 == 0)
      copyFromSurface(sprite[3], rand() % gl4dpGetWidth(), (rand() % gl4dpGetHeight())/3);
    gl4dpUpdateScreen(NULL);
    return;
  }
}
int n = 0;
void exemple_d_animation_06(int state) {
  const GLfloat s[2] = {1.0, -1.0}, t[2] = {0.0, 6.0};
  static GLfloat r1[] = {0.0, 0.0, 0.2, 0.5}, r2[] = {0, 0, 1, 1}, r = 0.0;
  static SDL_Surface * sprite[4];
  int i;
  static char * image[4] = {"./images/surf.png","./images/jetski.png","./images/planche-voile.png", "donus.png"};
  static SDL_Surface * sprite2 = NULL;
  static SDL_Surface * sprite3 = NULL;
  static GLuint s1 = 0, s2 = 0;
  GLfloat scale [2] = {1.0, 1.0};
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
  for (i = 0; i < 4; ++i){
    sprite[i] = IMG_Load(image[i]);
  }
    sprite2 = IMG_Load("plage.jpg");
    SDL_Surface * sc;
    sc = sprite2;
    sprite2 = SDL_ConvertSurfaceFormat(sc, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(sc);

    s1 = gl4dpInitScreen();
    //gl4dpClearScreenWith(0xFFFFFFFF);
    copyFromSurface(sprite2, 0, 0);
    gl4dpCopyFromSDLSurfaceWithTransforms(sprite2, scale, t);
    sprite3 = sprite[0];

    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    gl4dpSetScreen(s1);
    gl4dpDeleteScreen();
    gl4dpSetScreen(s2);
    gl4dpDeleteScreen();
    for(i = 0; i<4; i++){
      if(sprite[i]) {
        SDL_FreeSurface(sprite[i]);
        sprite[i] = NULL;
      }
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    s2 = gl4dpInitScreen();
    if(r1[2] > 1.5 && n < 3){
      r1[0] = 0;
      r1[2] = 0.2;
      n++;
      sprite3 = sprite[n];
    }
    gl4dpCopyFromSDLSurfaceWithTransforms(sprite3, s, t);
    gl4dpMap(s1, s2, r1, r2, 0.0);

    gl4dpSetScreen(s1);
    gl4dpClearScreenWith(0xFFFFFFFF);
    copyFromSurface(sprite2, 0, 0);
    gl4dpCopyFromSDLSurfaceWithTransforms(sprite2, scale, t);
    r1[0] += 0.01;
    r1[2] += 0.01;
    //r -= 0.00;
  
    gl4dpMap(s1, s2, r1, r2, r);
    gl4dpUpdateScreen(NULL);

    return;
  }

}

void animationsInit(void) {
  if(!_quadId)
    _quadId = gl4dgGenQuadf();
}
