#include "Print.hpp"
#include "math.h"
#include <QGLWidget>

/**
 * Draws a circle.
 *
 * @param x X-coordinates of the square
 * @param y Y-coordinates of the square

 * @param width Width of the screen
 * @param height of the screen
 */
void Print::drawCircle(int x, int y, float radius, int width, int height, float r, float g, float b){

  const float globX = ((float) x/width)*2-1;
  const float globY = (((float) y/height)*2-1);

  glBegin(GL_LINE_LOOP);
  glLineWidth(10.);
  glColor3f(r, g, b);
  for (int i=0; i < 360; i+=5)
  {
    float degInRad = (float)i / 360. * 2 * M_PI;
    glVertex2f(globX + cos(degInRad) * radius / (float)width, globY + sin(degInRad)*radius/(float)height);
  }
  glColor3f(1.0f, 1.0f, 1.0f);
  glLineWidth(1);
  glEnd();

}



/**
 * Draws a square around the cursor.
 * 
 * @param x X-coordinates of the square
 * @param y Y-coordinates of the square
 * @param sqrWidth Width of the square to print
 * @param sqrHeight Height of the square to print
 * @param width Width of the screen
 * @param height of the screen
 */
void Print::printSquare(int x, int y, float sqrWidth, float sqrHeight, int width, int height){

  const float globX = ((float) x/width)*2-1;
  const float globY = -(((float) y/height)*2-1);

  glBegin(GL_LINES);

  glColor3f(0.5f, 0.5f, 0.8f);

  glVertex2f(globX - sqrWidth, globY + sqrHeight);
  glVertex2f(globX + sqrWidth, globY + sqrHeight);
  glVertex2f(globX + sqrWidth, globY + sqrHeight);
  glVertex2f(globX + sqrWidth, globY - sqrHeight);
  glVertex2f(globX - sqrWidth, globY - sqrHeight);
  glVertex2f(globX + sqrWidth, globY - sqrHeight);
  glVertex2f(globX - sqrWidth, globY + sqrHeight);
  glVertex2f(globX - sqrWidth, globY - sqrHeight);

  glColor3f(1.0f, 1.0f, 1.0f);
  glEnd();
}


/**
 * Draws an obstacle set
 * 
 * @param Nx Size of the simulation matrix on the horizontal axis
 * @param Ny Size of the simulation matrix on the vertical axis
 * @param obst Set of obstacles to print
 */
void Print::drawObstacle(int Nx, int Ny, Obstacles &obst){

  std::list<Segment*> segList = obst.getSegList();
  std::list<Segment*>::iterator iter;
  float Ax, Ay, Bx, By, width;
  for(iter = segList.begin(); iter != segList.end(); iter++){
    Ax =  ((float) (*iter)->getA0()/Nx)*2;
    Ay =  ((float) (*iter)->getA1()/Ny)*2;
    Bx =  ((float) (*iter)->getB0()/Nx)*2;
    By =  ((float) (*iter)->getB1()/Ny)*2;
    width = ((float) (*iter)->getLength()/Nx)*2;

    glBegin(GL_LINES);

    glColor3f(0.5f, 1.0f, 1.0f);

    glVertex2f(-1.0 + Ax, -1.0 + Ay);
    glVertex2f(-1.0 + Bx, -1.0 + By);

    glVertex2f(-1.0 + Bx, -1.0 + By);
    glVertex2f(-1.0 + Bx + width, -1.0 + By);

    glVertex2f(-1.0 + Bx + width, -1.0 + By);
    glVertex2f(-1.0 + Ax + width, -1.0 + Ay);

    glVertex2f(-1.0 + Ax + width, -1.0 + Ay);
    glVertex2f(-1.0 + Ax, -1.0 + Ay);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
  }
}

void Print::reset(){}
void Print::pause(){}

Print::~Print(){}
