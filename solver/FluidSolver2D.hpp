#ifndef FLUIDSOLVER2D_HPP_
#define FLUIDSOLVER2D_HPP_

#include "FloatMatrix2D.hpp"
#include "Obstacles.hpp"
#include "../config.hpp"

/**
 * This class implements functions used to solve fluid equations.
 * Currently, only 2D fluids are considered.
 */

class FluidSolver{
public:
  FluidSolver(unsigned int i, unsigned int j); // Designed for testing
  FluidSolver(unsigned int i, unsigned int j, Config &config);
  ~FluidSolver();

  void velStep (FloatMatrix2D *u, FloatMatrix2D *v, FloatMatrix2D *u0, FloatMatrix2D *v0, float visc, float dt );
  void densStep (FloatMatrix2D *x, FloatMatrix2D *x0, FloatMatrix2D *u, FloatMatrix2D *v, float diff, float dt);

  void reset();
  void resetFluid();
  void resetSources();

  //private:
  inline void addSource ( FloatMatrix2D &x, FloatMatrix2D &s, float dt );
  void diffuse ( int b, FloatMatrix2D &x, FloatMatrix2D &x0, float diff, float dt);
  void advect ( int b, FloatMatrix2D &d, FloatMatrix2D &d0, FloatMatrix2D &u, FloatMatrix2D &v, float dt);
  void project ( FloatMatrix2D &u, FloatMatrix2D &v, FloatMatrix2D &p, FloatMatrix2D &div);
  void setBnd ( int b, FloatMatrix2D &x );

  FloatMatrix2D *_u, *_v, *_u_prev, *_v_prev;
  FloatMatrix2D *_dens, *_dens_prev;
  FloatMatrix2D *_dens_src;
  FloatMatrix2D *_u_vel_src, *_v_vel_src;

  Obstacles *_obstacles;
};

std::ostream &operator<< (std::ostream &stream, const FluidSolver &toPrint);


#endif
