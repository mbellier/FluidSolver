#include "SimplePrint.hpp"
#include <QGLWidget>
#include "math.h"


/**
 * This function set the color of a square given the scalar x 
 * and the velocity (u, v) at a specific point.
 * NB: the name of parameters u and v are omitted in this case
 * in order to avoid the "unused parameter" [-Wunused-parameter] error.
 */
void SimplePrint::applyColor(float x, float, float){
  glColor4f(1, 1, 1, x);  
}
