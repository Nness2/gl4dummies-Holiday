/*!\file window.c
 *
 * \brief Terre en GL / GL4Dummies + Texture + Lumière positionnelle +
 * Phong + Spéculaire
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <sys/timeb.h>
#include <time.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         resize(void);
static void         draw(void);

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief flag pour activer la lumière spéculaire */
static GLuint _specular = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1500.0, 20.0, 30.0, 1.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[3] = {0};
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};
static GLuint _cube = 0;

/*!\brief initialise les paramètres OpenGL */
static void init(void) {
  int i;
  SDL_Surface * t;
  static char * files[] = {"images/land_ocean_ice_2048.png", "images/land_ocean_ice_2048_glossmap.png", "images/sun.jpeg" };
  _pId  = gl4duCreateProgram("<vs>shaders/earth.vs", "<fs>shaders/earth.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");

  glGenTextures(3, _tId);
  for(i = 0; i < 3; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(files[i])) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", files[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(void) {
  GLint vp[4];
  int w, h;
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2]; h = vp[3];
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief dessine dans le contexte OpenGL actif. */

int uti = 0;
int uti2 = 0;
int tempo = 0, tempo2 = 0;
static void draw(void) {
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
  GLfloat dt = 0.0, lumPos[4], *mat;
  GLboolean cf, dp;
  struct timeb t1;
  ftime(&t1);
  if (uti == 0){
    tempo = (int)t1.time + 14;
    uti = 1;
  }
  resize();
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  cf = glIsEnabled(GL_CULL_FACE);
  dp = glIsEnabled(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  printf("t%d\n", tempo);
  printf("t1%d\n", (int)t1.time);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);

  if (tempo > (int)t1.time){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[2]);
    glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
    glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
    glUniform1i(glGetUniformLocation(_pId, "specular"), _specular);
    /* envoi de toutes les matrices stockées par GL4D */
    gl4duPushMatrix(); {
      gl4duRotatef(a0/3, 0, 1, 0);
      gl4duScalef(1.2, 1.2, 1.2);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    gl4dgDraw(_sphere);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _tId[1]);
    gl4duPushMatrix(); {
    gl4duRotatef(a0 / 2, 0, 1, 0);
    gl4duTranslatef(-6, 0, 2);  
    gl4duScalef(0.4, 0.4, 0.4);
    gl4duSendMatrices();
    } gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
    gl4dgDraw(_sphere);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[2]);
    gl4duPushMatrix(); {
    gl4duRotatef(a0 / 2, 0, 1, 0);
    gl4duTranslatef(-6, 0, 2);  
    gl4duScalef(0.01, 0.7, 0.01);
    gl4duSendMatrices();
    } gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
    gl4dgDraw(_cube);


    a0 += 360.0 * dt / 6.0;
  } 

  if (tempo < (int)t1.time){
    if (uti2 == 0){
      tempo2 = (int)t1.time + 16;
      uti2 = 1;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[2]);
    glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
    glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
    glUniform1i(glGetUniformLocation(_pId, "specular"), _specular);
    /* envoi de toutes les matrices stockées par GL4D */
    gl4duPushMatrix(); {
      gl4duRotatef(a0/3, 0, 1, 0);
      gl4duScalef(1.2, 1.2, 1.2);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    gl4dgDraw(_sphere);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _tId[1]);
    gl4duPushMatrix(); {
    if (tempo2 > (int)t1.time){
      gl4duTranslatef(-5, 0, 2);
      gl4duRotatef(-45, 0, 0, 1);
      gl4duRotatef(a0/4, 0, 1, 0);
      
    }
    else {
      gl4duTranslatef(-5, 0, 2);
      gl4duRotatef(a0/4, 0, 1, 0);
    }
    gl4duScalef(0.4, 0.4, 0.4);
    gl4duSendMatrices();
    } gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
    gl4dgDraw(_sphere);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[2]);
    gl4duPushMatrix(); {
    if (tempo2 > (int)t1.time){
      gl4duTranslatef(-5, 0, 2);  
      gl4duRotatef(-45, 0, 0, 1);
      gl4duRotatef(a0/4, 0, 1, 0);

    }
    else {
      gl4duTranslatef(-5, 0, 2);  
      gl4duRotatef(a0/4, 0, 1, 0);
    }
    gl4duScalef(0.01, 0.7, 0.01);
    gl4duSendMatrices();
    } gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
    gl4dgDraw(_cube);


    a0 += 360.0 * dt / 6.0;
  } 
  if(!cf) glDisable(GL_CULL_FACE);
  if(!dp) glDisable(GL_DEPTH_TEST);
}

void earth(int state) {
  /* INITIALISEZ VOS VARIABLES */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}