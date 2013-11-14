#ifndef CURVEPRINT_H
#define CURVEPRINT_H

#include "ColorPrint.hpp"
#include "../solver/FloatMatrix2D.hpp"

class CurvePrint : public ColorPrint {
public:
  CurvePrint();
private:
  inline virtual void applyColor(float x, float u, float v);
};

#endif
