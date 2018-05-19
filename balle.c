/*!\file window.c
 * \brief GL4D-Primitives et modèle simple de balle avec rebonds. Il
 * faut ajouter la prise en compte du temps, du rayon de la balle et
 * des bugs de type blocage de la balle (quand la gestion du temps sera
 * ajoutée).
 *
 * gestion de gravité, correction de bug et multi-balles ajoutée le 13/02/17
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 02 2017
 */
#include <assert.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>

//if (&& inc % 5 == 0)

GLuint _pId = 0;
GLuint _sphere = 0;
GLuint _tId = 0;

typedef struct mobile_t mobile_t;

struct mobile_t {
  GLfloat x, y, vx, vy, ax, ay;
  GLfloat r;
  GLfloat color[4];
};

static mobile_t * _mobiles = NULL;
static int _nbMobiles = 1;
static GLfloat _G[] = {0, -9.80};
static void drawBalle(void) ;
static void idle(void);



void cballe (int n){
  int i;
  _nbMobiles = n;
  _mobiles = malloc(_nbMobiles * sizeof *_mobiles);
  assert(_mobiles);

  for(i = 0; i < _nbMobiles; i++) {
    _mobiles[i].x = gl4dmSURand()/4-0.5;
    _mobiles[i].y = gl4dmSURand()-1;
    _mobiles[i].vx = gl4dmSURand() ;
    _mobiles[i].vy = gl4dmURand() / 4.0;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = 0.01 + gl4dmURand() / 10.0;
    _mobiles[i].color[0] = 0;
    _mobiles[i].color[1] = 0;
    _mobiles[i].color[2] = 0.5;
    _mobiles[i].color[3] = 0.3;
  }
}

void initBalle(int n) {
  _pId = gl4duCreateProgram("<vs>shaders/balle.vs", "<fs>shaders/balle.fs", NULL);
  _sphere=  gl4dgGenSpheref(10,10);
  glGenTextures(1, &_tId);
  SDL_Surface * t = IMG_Load("balle.png");
  assert(t);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
  glClearColor(0, 0, 0.5, 0);
  gl4duGenMatrix(GL_FLOAT, "mod");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  cballe(n);
  glClearColor(0, 0, 0.3, 0);
  

  // gl4duwMainLoop();
} 

int cmp = 0;
void mobileMove(void) {
  static GLuint t0 = 0;
  GLuint t;
  GLfloat dt;
  int i;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;
  for(i = 0; i < _nbMobiles; i++) {
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y -= _mobiles[i].vy * dt;
    _mobiles[i].vx += _mobiles[i].ax * dt;
    _mobiles[i].vy += _mobiles[i].ay * 0.001;
    if((_mobiles[i].x - _mobiles[i].r < -1.0 && _mobiles[i].vx < 0) || 
       (_mobiles[i].x + _mobiles[i].r >= 1.0 && _mobiles[i].vx > 0)) {
      _mobiles[i].vx = -_mobiles[i].vx;
      _mobiles[i].vx -= _mobiles[i].vx * 0.1;
      //_mobiles[i].vy -= _mobiles[i].vy * 0.05;
    }
  }
  if (cmp == 170){
    cballe(15);
    cmp = 0;
  }
  cmp ++;
}


void mobileDraw(void) {
  int i;
  gl4duBindMatrix("mod");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  for(i = 0; i < _nbMobiles; i++) {
    gl4dpFilledCircle(_mobiles[i].x, _mobiles[i].y, _mobiles[i].r);
    gl4duLoadIdentityf();
    gl4duTranslatef(_mobiles[i].x, _mobiles[i].y, 0);
    gl4duScalef(_mobiles[i].r, _mobiles[i].r, 1);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId, "color"), 1, _mobiles[i].color);
    gl4dgDraw(_sphere);
  }
}

static void idle(void) {
  mobileMove();
}

static void drawBalle(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pId);
  mobileDraw();
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés.*/


void balle(int state) {
  switch(state) {
  case GL4DH_INIT:
  glClearColor(0, 0, 0.3, 0);
    initBalle(15);
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    glClearColor(0, 0, 0.3, 0);
    gl4duwIdleFunc(idle);
    drawBalle();
    return;
  }
}
