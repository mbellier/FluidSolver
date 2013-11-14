#ifndef SIMPLEPRINT_H
#define SIMPLEPRINT_H

#include "ColorPrint.hpp"

class SimplePrint : public ColorPrint {
private:
  void applyColor(float x, float u, float v);
};

#endif // SIMPLEPRINT_H
