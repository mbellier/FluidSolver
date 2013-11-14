#include "CurvePrint.hpp"
#include "math.h"
#include <QGLWidget>

CurvePrint::CurvePrint(){}

#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define N 40
#define NORMALIZE(x) ((float)((int) (x * N)) / N)

/**
 * This function set the color of a square given the scalar x 
 * and the velocity (u, v) at a specific point.
 */
void CurvePrint::applyColor(float x, float u, float v){
  float x_  = x;

  if (x_ - NORMALIZE(x_) > .01){
    x_ = 0;
  }

  glColor4f(0, 1, (u * u + v * v)*400, x_);  
  
}


