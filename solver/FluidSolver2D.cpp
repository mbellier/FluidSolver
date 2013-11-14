#include "FluidSolver2D.hpp"


#define SWAP(x0,x) {FloatMatrix2D *tmp = x0; x0 = x; x = tmp;} // Uses pointers

/** Constructor
 */
FluidSolver::FluidSolver(unsigned int i, unsigned int j, Config &config){
  _u         = new FloatMatrix2D (i, j);
  _v         = new FloatMatrix2D (i, j);
  _u_prev    = new FloatMatrix2D (i, j);
  _v_prev    = new FloatMatrix2D (i, j);
  _dens      = new FloatMatrix2D (i, j);
  _dens_prev = new FloatMatrix2D (i, j);
  _dens_src  = new FloatMatrix2D (i, j);
  _u_vel_src = new FloatMatrix2D (i, j);
  _v_vel_src = new FloatMatrix2D (i, j);
  _obstacles = new Obstacles(i,j,config);
}

FluidSolver::FluidSolver(unsigned int i, unsigned int j){
  _u         = new FloatMatrix2D (i, j);
  _v         = new FloatMatrix2D (i, j);
  _u_prev    = new FloatMatrix2D (i, j);
  _v_prev    = new FloatMatrix2D (i, j);
  _dens      = new FloatMatrix2D (i, j);
  _dens_prev = new FloatMatrix2D (i, j);
  _dens_src  = new FloatMatrix2D (i, j);
  _u_vel_src = new FloatMatrix2D (i, j);
  _v_vel_src = new FloatMatrix2D (i, j);
  _obstacles = new Obstacles();
}


FluidSolver::~FluidSolver(){
  delete _u;
  delete _v;
  delete _u_prev;
  delete _v_prev;
  delete _dens;
  delete _dens_prev;
  delete _dens_src;
  delete _obstacles;
}

/**
 * Update density matrix from a given source matrix.
 * @param x density matrix
 * @param s source matrix
 * @param dt time interval
 */
void FluidSolver::addSource ( FloatMatrix2D &x, FloatMatrix2D &s, float dt ){
  x.addAndMultiply(s, dt);
}

/**
 * Diffusion of the density of particule.
 * @param N Dimension of the NxN matrix
 * @param b Enumeration describing the boundary conditions (0 : no wrapping, 1 : x-wrap,  : y-wrap)
 * @param x density matrix at t
 * @param x0 density matrix at t-dt
 * @param diff Diffusion coefficient
 * @param dt time interval
 */
void FluidSolver::diffuse ( int b, FloatMatrix2D &x, FloatMatrix2D &x0, float diff, float dt){
  unsigned int i, j, k;
  float a = dt * diff * (x.getSize(0)-2) * (x.getSize(1)-2);

  for ( k=0 ; k < 10; k++) {
    for ( i=1 ; i <= x.getSize(1)-2 ; i++ ){
      for ( j=1 ; j <= x.getSize(0)-2 ; j++ ){
        if(!(_obstacles->isInObstacles(i,j))){
	if(a == 0)
	  x.set(i,j, x0.get(i,j));
	else
	  x.set(i,j, (x0.get(i,j) + a*(x.get(i-1,j)+x.get(i+1,j)+ x.get(i,j-1)+x.get(i,j+1)))/(1+4*a));
}
      }
    }
    setBnd (b, x);
  }
}


/**
 * Advection, ie. movement of the density of particules along the velocity field.
 * @param N Dimension of the NxN matrix
 * @param b Enumeration describing the boundary conditions (0 : no wrapping, 1 : x-wrap,  : y-wrap)
 * @param d density matrix at t
 * @param d0 density matrix at t-dt
 * @param u field vector first coordinate at t-dt
 * @param v field vector second coordinate at t-dt
 * @param diff Diffusion coefficient
 * @param dt time interval
 */
void FluidSolver::advect (int b, FloatMatrix2D &d, FloatMatrix2D &d0, FloatMatrix2D &u, FloatMatrix2D &v, float dt ){
  unsigned int i, j, i0, j0, i1, j1;
  float x, y, s0, t0, s1, t1;
  const unsigned int N_i = d.getSize(1) - 2;
  const unsigned int N_j = d.getSize(0) - 2;

  const unsigned int  dt0_x = dt * N_i;
  const unsigned int  dt0_y = dt * N_j;

  for (i=1; i <= N_i; i++){
    for (j=1; j <= N_j; j++){
      if(!(_obstacles->isInObstacles(i,j))){
      x = i - dt0_x * u.get(i,j);
      y = j - dt0_y * v.get(i,j);

      if (x < 0.5) x = 0.5;
      if (x > N_i + 0.5) x = N_i + 0.5;

      i0 = (int)x; i1 = i0 + 1;

      if (y < 0.5) y = 0.5;
      if (y > N_j + 0.5) y = N_j + 0.5;

      j0 = (int)y; j1 = j0 + 1;

      s1 = x - i0;
      s0 = 1 - s1;
      t1 = y - j0;
      t0 = 1 - t1;

      if(!(_obstacles->isInObstacles(i0,j0))
	 && !(_obstacles->isInObstacles(i1,j1))
	 && !(_obstacles->isInObstacles(i0,j1))
	 && !(_obstacles->isInObstacles(i1,j0))){
        d.set(i,j, s0 * (t0 * d0.get(i0,j0) + t1 * d0.get(i0,j1)) + s1 * (t0 * d0.get(i1,j0) + t1 * d0.get(i1,j1)));
      }
      else{
        d.set(i,j, d0.get(i,j));
      }
    }
    }
  }
  setBnd (b, d);
}


/**
 * Updates the density during a step of dt.
 */
void FluidSolver::densStep ( FloatMatrix2D *x, FloatMatrix2D *x0, FloatMatrix2D *u, FloatMatrix2D *v, float diff, float dt){
  addSource (*x, *x0, dt);
  SWAP (x0, x); diffuse (0, *x, *x0, diff, dt );
  SWAP (x0, x); advect  (0, *x, *x0, *u, *v, dt );
}


/**
 * Projection, ie. computation of the velocity field.
 */
void FluidSolver::project (FloatMatrix2D &u, FloatMatrix2D &v, FloatMatrix2D &p, FloatMatrix2D &div )
{
  unsigned int i, j, k;
  float h_u, h_v;

  h_u = 1.0 / (u.getSize(1)-2);
  h_v = 1.0 / (v.getSize(1)-2);

  for ( i=1 ; i <= u.getSize(1) - 2; i++ ) {
    for ( j=1 ; j <= u.getSize(0) - 2; j++ ) {
      if(!(_obstacles->isInObstacles(i,j))){
      div.set(i,j, -0.5 * h_u * (u.get(i+1,j) - u.get(i-1,j)) - 0.5 * h_v * (v.get(i,j+1) - v.get(i,j-1)));
      p.set(i, j, 0);
    }
    }
  }
  setBnd (0, div); setBnd (0, p);

  for ( k = 0 ; k < 10 ; k++ ) {
    for ( i = 1; i <= u.getSize(1) - 2; i++ ) {
      for (j = 1; j <= u.getSize(0) - 2; j++ ) {
        if(!(_obstacles->isInObstacles(i,j))){
	p.set(i,j, (div.get(i,j) + p.get(i-1,j) + p.get(i+1,j) + p.get(i,j-1) + p.get(i,j+1)) / 4);
}
      }
    }
    setBnd (0, p);
  }
  for ( i=1 ; i <= u.getSize(1) - 2; i++ ) {
    for ( j=1 ; j <= u.getSize(0) - 2; j++ ) {
      if(!(_obstacles->isInObstacles(i,j))){
      u.set(i,j, u.get(i,j) - 0.5 * (p.get(i+1,j) - p.get(i-1,j)) / h_u);
      v.set(i,j, v.get(i,j) - 0.5 * (p.get(i,j+1) - p.get(i,j-1)) / h_v);
    }
    }
  }
  setBnd (1, u); setBnd (2, v);
}

/**
 * Updates the velocity field during a step of dt.
 */
void FluidSolver::velStep (FloatMatrix2D *u, FloatMatrix2D *v, FloatMatrix2D *u0, FloatMatrix2D *v0, float visc, float dt ){
  addSource (*u, *u0, dt);
  addSource (*v, *v0, dt);
  SWAP (u0, u); diffuse (1, *u, *u0, visc, dt);
  SWAP (v0, v); diffuse (2, *v, *v0, visc, dt);

  project (*u, *v, *u0, *v0);
  SWAP (u0, u); SWAP (v0, v);
  advect (1, *u, *u0, *u0, *v0, dt);
  advect (2, *v, *v0, *u0, *v0, dt);
  project (*u, *v, *u0, *v0);
}

/**
 * Applies the Boundary conditions.
 */
void FluidSolver::setBnd (int b, FloatMatrix2D &x ) {
  unsigned int i, N_x = x.getSize(1), N_y = x.getSize(0);

  if (b<3){
    for (i = 1; i < N_x-1; i++){
      x.set(i,   0,      b==2 ? -x.get(i,1)      : x.get(i, 1));
      x.set(i, N_y - 1, b==2 ? -x.get(i,N_y-2)  : x.get(i, N_y-2));
    }
    for (i = 1; i < N_y-1; i++) {
      x.set(0,   i, b==1 ? -x.get(1,i)      : x.get(1, i));
      x.set(N_x - 1, i, b==1 ? -x.get(N_x-2, i) : x.get(N_x-2, i));
    }
  }
  else if(b == 3){ //loop (diffusion in densStep)
    for (i = 1; i < N_x-1; i++){
      x.set(i,   0, x.get(i, N_y-2));
      x.set(i, N_y - 1, x.get(i, 1));
    }
    for (i = 1; i < N_y-1; i++) {
      x.set(0,   i, x.get(N_x-2, i));
      x.set(N_x - 1, i, x.get(1, i));
    }
    x.set(0,   0, 0.5 * (x.get(1,       0 )        + x.get(0,       1)));
    x.set(0,   N_y - 1, 0.5 * (x.get(1,       N_y - 1)   + x.get(0,       N_y - 2)));
    x.set(N_x - 1, 0, 0.5 * (x.get(N_x - 2, 0)         + x.get(N_x - 1, 1)));
    x.set(N_x - 1, N_y - 1, 0.5 * (x.get(N_x - 2, N_y - 1)   + x.get(N_x - 1, N_y - 2)));
    return;
  }

  if(b == 4){ // well (diffusion in densStep)
    for (i = 1; i < N_x-1; i++){
      x.set(i, 0, 0);
      x.set(i, N_y - 1, 0);
    }
    for (i = 1; i < N_y-1; i++) {
      x.set(0, i, 0);
      x.set(N_x - 1, i , 0);
    }
    x.set(0, 0, 0);
    x.set(0, N_y - 1, 0);
    x.set(N_x - 1, 0, 0);
    x.set(N_x - 1, N_y - 1, 0);
    return;
  }

  x.set(0, 0, 0.5 * (x.get(1, 0 ) + x.get(0,   1)));
  x.set(0, N_y - 1, 0.5 * (x.get(1, N_y - 1)
			   + x.get(0, N_y - 2)));
  x.set(N_x - 1, 0, 0.5 * (x.get(N_x - 2, 0)
			   + x.get(N_x - 1, 1)));
  x.set(N_x - 1, N_y - 1, 0.5 * (x.get(N_x - 2, N_y - 1) 
				 + x.get(N_x - 1, N_y - 2)));

  _obstacles->setObstacles(b, x);
}

void FluidSolver::resetFluid(){
  _u->fill(0);
  _v->fill(0);
  _u_prev->fill(0);
  _v_prev->fill(0);
  _dens->fill(0);
  _dens_prev->fill(0);
}

void FluidSolver::resetSources(){
  _dens_src->fill(0);
  _u_vel_src->fill(0);
  _v_vel_src->fill(0);
}

void FluidSolver::reset(){
  resetFluid();
  resetSources();
}

std::ostream &operator<< (std::ostream &stream, const FluidSolver &toPrint){
  stream << *(toPrint._dens);
  return stream;
}
