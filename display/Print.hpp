#ifndef PRINT_H
#define PRINT_H

#include "../solver/FloatMatrix2D.hpp"
#include "../solver/Obstacles.hpp"

class Print
{
public:
  virtual void printMatrixScalar(FloatMatrix2D &scalar, FloatMatrix2D &u, FloatMatrix2D &v) = 0;
  virtual void printMatrixVector(FloatMatrix2D &u, FloatMatrix2D &v) = 0;
  virtual void reset();
  virtual void pause();

  void drawCircle(int x, int y, float radius, int width, int height, float r, float g, float b);
  void printSquare(int x, int y, float sqrWidth, float sqrHeight, int width, int height);
  void drawObstacle(int Nx, int Ny, Obstacles &);
 
  virtual ~Print();
};

#endif
