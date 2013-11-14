#ifndef PARTICULESPRINT_H
#define PARTICULESPRINT_H

#include "ColorPrint.hpp"
#include "../solver/FloatMatrix2D.hpp"

#define PARTICLESPRINT__MOVEMENT_FACTOR 80

#define PARTICLESPRINT__TRAIL_LENGTH    100

class ParticlesPrint : public ColorPrint {
public:
  ParticlesPrint(const unsigned int nb_particles,
		  const unsigned int xMax,
		  const unsigned int yMax,
		  const bool antialiasing = false,
		  const bool enableTrail  = false);
  ~ParticlesPrint();
  void printMatrixScalar(FloatMatrix2D &scalar, FloatMatrix2D &u, FloatMatrix2D &v);
  void reset();  
  void pause();

private:
  inline virtual void applyColor(float x, float u, float v);
  void drawLine(float x, float y, float x2, float y2, 
		float n, float m, int width, 
		float r, float g, float b, float a);

  class TrailParticle {
  public:
    TrailParticle();    
    inline void move(const float vx, const float vy);
    float _x;
    float _y;
    float _vx;
    float _vy;
  };
  
  class Particle : public TrailParticle {
  public:
    Particle();
    ~Particle();
    inline void reset(const float x, const float y);
    TrailParticle *_trail;
  };

  Particle *_particles;
  unsigned int _nb_particles;

  const unsigned int _xMax, _yMax;
  const bool _enableTrail;
  bool _pause;
  
};

#endif
