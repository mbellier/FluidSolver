#include "ColorPrint.hpp"
#include "math.h"
#include <QGLWidget>

ColorPrint::ColorPrint(bool antialiasing)
  : _antialiasing(antialiasing)
{}

#define MAX(a, b) ((a) < (b) ? (b) : (a))

/**
 * Display a set of vectors which origin is situated at
 * of cells uniformly distributed on the window. The coordinates
 * are stored in 2 matrix (x values and y values) for a 2D
 * reprentation. The two matrix have the same dimensions.
 *
 * @param X Matrix storing the X-coordinates of the vectors to display
 * @param Y Matrix storing the Y-coordinates of the vectors to display
 */
void ColorPrint::printMatrixVector(FloatMatrix2D &X, FloatMatrix2D &Y){
  unsigned int i,j;
  const float n = X.getSize(0);//height
  const float m = X.getSize(1);//width

  const float treshold = .1;
  const float rescale  = 10;

  /* For each cells of the two matrix, two points of the
   * vectors are defined.
   */
  for (i = 0; i < m; ++i){
    for (j = 0; j < n; ++j){
      /* Drawing of a vector */
      glBegin(GL_LINES);
      glColor3f(0.0f, 0.2f, 1.0f);

      /* origin of the vector */
      glVertex2f(-1.0 + (1/m) + i*(2/m),	\
		 -1.0 + (1/n) + j*(2/n));

      /* Extremity of the vector */
      float normX = X.get(i,j) * rescale;
      float normY = Y.get(i,j) * rescale;
      normX = normX > treshold  ? treshold  : normX;
      normX = normX < -treshold ? -treshold : normX;
      normY = normY > treshold  ? treshold  : normY;
      normY = normY < -treshold ? -treshold : normY;

      glVertex2f(-1.0 + (1/m) + i*(2/m) + normX,	\
		 -1.0 + (1/n) + j*(2/n) + normY);

      glColor3f(1.0f, 1.0f, 1.0f);
      glEnd();

      /* Drawing of the origin point */
      glBegin(GL_POINTS);
      /* green color */
      glColor3f(0.0f, 1.0f, 0.0f);
      glVertex2f(-1.0 + (1/m) + i*(2/m), -1.0 + (1/n) + j*(2/n));
      glColor3f(1.0f, 1.0f, 1.0f);
      glEnd();
    }
  }

#if (COLOR_PRINT__ENABLE_GRID) // see header
  /* Displays a grid to visualize each cells.
   */
  for (i = 0; i < n-1; ++i){
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.0f, 0.3f);
    glVertex2f(-1.0, 1.0 - (2/n) - i*(2/n));
    glVertex2f(1.0, 1.0 - (2/n) - i*(2/n));
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
  }
  for (i = 0; i < m-1; ++i){
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.0f, 0.3f);
    glVertex2f(1.0 - (2/m) - i*(2/m), 1.0);
    glVertex2f(1.0 - (2/m) - i*(2/m), -1.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
  }
#endif
}

/**
 * This function set the color of a square given the scalar x 
 * and the velocity (u, v) at a specific point.
 *
 * @param x Parameter of the choice of the color
 */
void ColorPrint::applyColor(float x, float , float ){
  const float treshold1 = 1.;
  const float treshold2 = 4.;
  const float treshold3 = 10.;

  /* red */
  if (x < treshold1) {
    glColor4f(CLAMP(x, 0., treshold1), 0., 0., x);
  }
  
  /* yellow */
  else if (x < treshold2) {
    glColor4f(1., CLAMP(x, treshold1, treshold2) - treshold1, 0., 1.);
  }
  
  /* white */
  else if (x < treshold3){
    glColor4f(1., 1., CLAMP(x, treshold2, treshold3) - treshold2, 1.); 
  }

  else{
    glColor4f(1., 1., 1., 1.); 
  }

}

/** 
 * Display a set of squares which represent the density
 * of particules on the area stored in the matrix X. More
 * white square is the more particles are. We use transparency
 * to make the gradient.
 */
void ColorPrint::printMatrixScalar(FloatMatrix2D &X,FloatMatrix2D &u,FloatMatrix2D &v){
  unsigned int i,j;
  const float n = X.getSize(0);//height
  const float m = X.getSize(1);//width

  // For each cell of the matrix, draw a square
  for (i = 0; i < m; i += 1){
    for (j = 0; j < n; j += 1){
      //Draws a square by defining four points.
      glBegin(GL_QUADS);
      //Defines the color of the square
      applyColor(X.get(i,j), u.get(i,j), v.get(i,j));

      glVertex2f(-1.0 + i*(2/m), -1.0 + j*(2/n));

      if (_antialiasing){
	if(i < m-1)
	  applyColor(X.get(i+1,j), u.get(i+1,j), v.get(i+1,j));
	else
	  applyColor(X.get(i,j), u.get(i,j), v.get(i,j));
      }
      glVertex2f(-1.0 + (2/m)*(i + 1), -1.0 + j*(2/n));

      if (_antialiasing){
	if(i < m-1 && j < n-1)
	  applyColor(X.get(i+1,j+1), u.get(i+1,j+1), v.get(i+1,j+1));
	else
	  applyColor(X.get(i,j), u.get(i,j), v.get(i,j));
      }
      glVertex2f(-1.0 + (2/m)*(i + 1), -1.0 + (2/n)*(j + 1));

      if (_antialiasing){
	if(j < n-1)
	  applyColor(X.get(i,j+1), u.get(i,j+1), v.get(i,j+1));
	else
	  applyColor(X.get(i,j), u.get(i,j), v.get(i,j));
      }
      glVertex2f(-1.0 + i*(2/m), -1.0 + (2/n)*(j + 1));

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glEnd();
    }
  }
}

