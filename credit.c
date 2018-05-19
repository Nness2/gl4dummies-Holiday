/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
//static void initData(void);
//static int random_range (int min, int max);
void resize (void);
//static void draw(void);
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0;
static GLuint _tId[2]={0,0};
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/


/*!\brief initialise les données */
static void initShark(void) {
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/base.vs", "<fs>shaders/base.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
   int i;
  SDL_Surface * t;
  static char * file[] = {"./images/fondmarin.jpg", "./images/credit.png"};
  glGenTextures(2, _tId);
  for(i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(file[i])) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", file[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();
}

void resize2(void) {
  GLint vp[4];
  int w, h;
  glGetIntegerv(GL_VIEWPORT, vp);
  w = vp[2]; h = vp[3];
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}


static void drawShark(void) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  static GLuint frame=0;
  static GLfloat a = -4;
  resize2();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
    glBindTexture(GL_TEXTURE_2D, _tId[0]);
 
  gl4duTranslatef(0 , 2, -9.0);
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -2.0, 0.0);
    gl4duRotatef(-180, 1, 0, 0);
    gl4duScalef(6, 4, 4);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "toto"), 0);
 // glUniform4fv(glGetUniformLocation(_pId, "toto"),1);
  
  gl4dgDraw(_quad);



gl4duPushMatrix(); {
    gl4duTranslatef(0,-2, 2);
    if (a < 0.3)
      gl4duTranslatef(a+=0.01,0,0);
    else
      gl4duTranslatef(a,0,0);
    gl4duRotatef(0, 0, 0, 0);
    gl4duScalef(3.7,1.8,3);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "toto"), 0);
  glBindTexture(GL_TEXTURE_2D,_tId[1]);
 // glUniform4fv(glGetUniformLocation(_pId, "toto"), 1, rouge);
  gl4dgDraw(_quad);


  frame++;
  //glUniform4fv(glGetUniformLocation(_pId, "toto"), 1, bleu);
  //gl4dgDraw(_quad);

}

void credit(int state) {
  switch(state) {
  case GL4DH_INIT:
    initShark();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    drawShark();
    return;
  }
}