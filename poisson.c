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
#include <stdio.h>
#include <assert.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>
GLuint _pId2 = 0;
GLuint _quad = 0;
GLuint _tId2[2] = {0,0};
typedef struct mobile_t mobile_t;
void idle(void);
struct mobile_t {
  GLfloat x, y, vx, vy, ax, ay;
  GLfloat r;
  GLfloat color[4];
};

static mobile_t * _mobiles = NULL;
static mobile_t * _bulles = NULL;
static int _nbMobiles = 1;
static GLfloat _G[] = {0, -9.80};

void initPoisson(int n) {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  int i;
  _pId2 = gl4duCreateProgram("<vs>shaders/poisson.vs", "<fs>shaders/poisson.fs", NULL);
  _quad = gl4dgGenQuadf();
  glGenTextures(2, _tId2);
  SDL_Surface * t[2];
  SDL_Surface * poisson= IMG_Load("poisson.png");
  SDL_Surface * bulle= IMG_Load("bulle.png");
  t[0] = poisson;
  t[1] = bulle;
  assert(t);
  for (i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, _tId2[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t[i]->w, t[i]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[i]->pixels);
  }
  glClearColor(0, 0, 0.1, 0);
  gl4duGenMatrix(GL_FLOAT, "mod");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
  //mobileInit(30);
  _nbMobiles = n;
  _mobiles = malloc(_nbMobiles * sizeof *_mobiles);
  _bulles = malloc(60 * sizeof *_mobiles);
  assert(_mobiles);
  
  for(i = 0; i < _nbMobiles; i++) {
    _mobiles[i].x = gl4dmSURand()-1;
    _mobiles[i].y = gl4dmSURand()-1;
    _mobiles[i].vx = gl4dmSURand()*1.5;
    _mobiles[i].vy = gl4dmURand() / 4.0;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = 0.1 + gl4dmURand() / 10.0;
    _mobiles[i].color[0] = gl4dmURand();
    _mobiles[i].color[1] = gl4dmURand();
    _mobiles[i].color[2] = gl4dmURand();
    _mobiles[i].color[3] = 1;
  }

  for(i = 0; i <60; i++) {
    _bulles[i].x = 1+gl4dmSURand()-1;
    _bulles[i].y = gl4dmSURand()-1;
    _bulles[i].vx = gl4dmSURand() / 4.0;
    _bulles[i].vy = gl4dmURand()*2 ;
    _bulles[i].ax = _G[0];
    _bulles[i].ay = _G[1];
    _bulles[i].r = 0.01 - gl4dmURand() / 10.0;
    _bulles[i].color[0] = 0;
    _bulles[i].color[1] = 0;
    _bulles[i].color[2] = 1;
    _bulles[i].color[3] = 0.7;
  }
  //glClearColor(0, 0, 0.3, 0);
} 

void mobileMove2(void) {
  static GLuint t0 = 0;
  GLuint t;
  GLfloat dt;
  int i;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;
  for(i = 0; i < _nbMobiles; i++) {
    if(_mobiles[i].x > 1)
      _mobiles[i].x = -1.5;
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y -= _mobiles[i].vy * dt/5;
    //_mobiles[i].vx += _mobiles[i].ax +0.01;
    _mobiles[i].vy += _mobiles[i].ay * dt/100;
  }
  for(i = 0; i < 60; i++) {
    if(_bulles[i].y > 1)
      _bulles[i].y = -1;
    _bulles[i].x += _bulles[i].vx * dt;
    _bulles[i].y += _bulles[i].vy * dt;
    _bulles[i].vx += _bulles[i].ax * dt;
   // _bulles[i].vy += _bulles[i].ay * 0.01;
  }


}

void poissonDraw(void) {
  int i;
  gl4duBindMatrix("mod");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId2[0]);
  glUniform1i(glGetUniformLocation(_pId2, "tex"), 0);
  for(i = 0; i < _nbMobiles; i++) {
    //gl4dpFilledCircle(_mobiles[i].x, _mobiles[i].y, _mobiles[i].r);
    gl4duLoadIdentityf();
    gl4duTranslatef(_mobiles[i].x, _mobiles[i].y, 0);
    gl4duScalef(_mobiles[i].r, _mobiles[i].r, 1);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId2, "color"), 1, _mobiles[i].color);
    gl4dgDraw(_quad);
  }
}

void bulleDraw(void) {
  int i;
  gl4duBindMatrix("mod");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId2[1]);
  glUniform1i(glGetUniformLocation(_pId2, "tex"), 0);
  for(i = 0; i < _nbMobiles; i++) {
    //gl4dpFilledCircle(_bulles[i].x, _bulles[i].y, _bulles[i].r);
    gl4duLoadIdentityf();
    gl4duTranslatef(_bulles[i].x, _bulles[i].y, 0);
    gl4duScalef(_bulles[i].r, _bulles[i].r, 1);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId2, "color"), 1, _bulles[i].color);
    gl4dgDraw(_quad);
  }
}

void idle(void) {
  mobileMove2();
}

void drawPoisson(void) {
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pId2);
  poissonDraw();
  bulleDraw();
}

/*!\brief créé la fenêtre, un screen 2D, place la fonction display et
 * appelle la boucle principale.*/
// int main(int argc, char ** argv) {
//    fenêtre positionnée en (10, 10), ayant une dimension de (512, 512) et un titre "GL4D-Primitives" 
//   int i;

//   atexit(quit);


//   return 0;
// }

void poisson(int state) {
  switch(state) {
  case GL4DH_INIT:
  //glClearColor(0, 0, 0.3, 0);
    initPoisson(30);
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    //glClearColor(0, 0, 0.3, 0);
    gl4duwIdleFunc(idle);
    drawPoisson();

    return;
  }
}