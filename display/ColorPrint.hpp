#ifndef COLORPRINT_H
#define COLORPRINT_H

#include "Print.hpp"
#include "../solver/FloatMatrix2D.hpp"

#define COLOR_PRINT__ENABLE_GRID 0

#define CLAMP(v, a, b) (a + (v - a) / (b - a))

class ColorPrint : public Print {
public:
  ColorPrint(bool antialiasing = false);
  virtual void printMatrixScalar(FloatMatrix2D &scalar, FloatMatrix2D &u, FloatMatrix2D &v);
  void printMatrixVector(FloatMatrix2D &u, FloatMatrix2D &v) ;
private:
  inline virtual void applyColor(float x, float u, float v);
  bool _antialiasing;
};

#endif
