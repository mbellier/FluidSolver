#ifndef OBSTACLES_HPP_
#define OBSTACLES_HPP_

#include "Segment.hpp"
#include "../config.hpp"
/**
 * This class implements a set of segments in the middle of the fluid.
 */

class Obstacles{
  int _N_x;
  int _N_y;
  std::list<Segment*> segList;
public:
  Obstacles(unsigned int, unsigned int, Config &);
  Obstacles(); // Designed for testing
  ~Obstacles();

  void setObstacles(int, FloatMatrix2D &);
  void addSegment(unsigned int A0, unsigned int A1, \
    unsigned int B0, unsigned int B1, unsigned int L0);
  void reset();
  
  inline bool isInObstacles(unsigned int i, unsigned int j){
    bool res = false;
    std::list<Segment*>::iterator iter;
    for(iter = segList.begin(); iter != segList.end(); iter++){
      res |= (*iter)->isInSegment(i,j);
    }
    return res;
  }
  std::list<Segment*> getSegList(){return segList;};
};


#endif
