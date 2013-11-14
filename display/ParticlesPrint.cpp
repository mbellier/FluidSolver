#include "ParticlesPrint.hpp"
#include "math.h"
#include <QGLWidget>

ParticlesPrint::TrailParticle::TrailParticle() {
  _x  = 0.;
  _y  = 0.;
  _vx = 0.;
  _vy = 0.;
}

ParticlesPrint::Particle::Particle(){
  _trail = new TrailParticle [PARTICLESPRINT__TRAIL_LENGTH];
}

ParticlesPrint::Particle::~Particle(){
  delete[] _trail;
}

/**
 * Resets the position of a particle to a specific position
 * 
 * @param x X-coordinates of the new position of the particle
 * @param y Y-coordinates of the new position of the particle
 */
void ParticlesPrint::Particle::reset(const float x, const float y){
  _x = 0;//x;
  _y = 0;//y;
  for(unsigned int k = 0; k < PARTICLESPRINT__TRAIL_LENGTH; k++){
    _trail[k]._x = x;
    _trail[k]._y = y;
  }
}

/**
 * Moves a particle to a new position
 * 
 * @param vx X-coordinates of the velocity vector to follow
 * @param vy Y-coordinates of the velocity vector to follow
 */
void ParticlesPrint::TrailParticle::move(const float vx, const float vy){
  _vx = vx * PARTICLESPRINT__MOVEMENT_FACTOR;
  _vy = vy * PARTICLESPRINT__MOVEMENT_FACTOR;
  _x += _vx;
  _y += _vy;
}


ParticlesPrint::ParticlesPrint(const unsigned int nb_particles, 
				 const unsigned int xMax,
				 const unsigned int yMax,
				 const bool antialiasing,
				 const bool enableTrail)
  :  ColorPrint(antialiasing), 
     _nb_particles(nb_particles),
     _xMax(xMax),
     _yMax(yMax),
     _enableTrail(enableTrail),
     _pause(false)
{

  _particles = new Particle[_nb_particles];

  reset();

}

/**
 * Resets randomly the position of the particules
 */
void ParticlesPrint::reset() {

  for (unsigned int i = 0; i < _nb_particles; i++){
    _particles[i].reset(rand() / (float)RAND_MAX * _xMax,
			 rand() / (float)RAND_MAX * _yMax);
  }

}

ParticlesPrint::~ParticlesPrint(){
  delete[] _particles;
}


/**
 * This function set the color of a square given the scalar x 
 * and the velocity (u, v) at a specific point.
 */
void ParticlesPrint::applyColor(float x, float u, float v){
  glColor4f(0, 1, (u * u + v * v)*400, x);  
}

void ParticlesPrint::drawLine(float x, float y, float x2, float y2, 
	      float n, float m, int width, 
	      float r, float g, float b, float a ){
 
    glColor4f( r, g, b, a);
    glLineWidth(width);

    glBegin(GL_LINES);

    glVertex2f( -1.0 + x  * (2 / m), -1.0 + y  * (2 / n));
    glVertex2f( -1.0 + x2 * (2 / m), -1.0 + y2 * (2 / n));

    glEnd();
    
    glLineWidth(1);
    glColor4f(0, 0, 0, 1);
}  

void ParticlesPrint::printMatrixScalar(FloatMatrix2D &X,FloatMatrix2D &u,FloatMatrix2D &v){
  ColorPrint::printMatrixScalar(X, u, v);

  const float n = X.getSize(0);//height
  const float m = X.getSize(1);//width

  float x, y;
  unsigned int ix, iy;

  /* for each particle */

  for (unsigned int i = 0; i < _nb_particles; i ++){

    /* particule position */
    x = _particles[i]._trail[0]._x;
    y = _particles[i]._trail[0]._y;

    ix = (unsigned int) x; 
    iy = (unsigned int) y;

    /* ignore out of bounds particles */
    if (!(ix < m && iy < n)) continue;
    
    /* match particles with density */
    //if (X.get( (int) x, (int) y) < .5) continue;


    /* prevent particles to move when simulation is paused */
    if (!_pause){

      /* update trail */
      if (_enableTrail){
	for(unsigned int j = PARTICLESPRINT__TRAIL_LENGTH-1; j > 0; j--){
	  _particles[i]._trail[j]._x = _particles[i]._trail[j - 1]._x;
	  _particles[i]._trail[j]._y = _particles[i]._trail[j - 1]._y;
	}
      }

      /* move particles */
      _particles[i]._trail[0].move( u.get( (int) x, (int) y), 
				    v.get( (int) x, (int) y));
    }

    /* particle drawing */
    glBegin(GL_POINTS);
    
    for(unsigned int j = 0; j < PARTICLESPRINT__TRAIL_LENGTH; j++){

      x = _particles[i]._trail[j]._x;
      y = _particles[i]._trail[j]._y;
      
      glColor4f(1.0f, 1.0f, 1.0f, 1.5/ ((float)sqrt((float)j+1)));
      glVertex2f(-1.0 + x*(2/m), -1.0 + y*(2/n));

      if (!_enableTrail) break;
    }
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnd();
    
  }
}

void ParticlesPrint::pause(){
  _pause = !_pause;
}
