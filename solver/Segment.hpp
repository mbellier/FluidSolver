#ifndef SEGMENT_HPP_
#define SEGMENT_HPP_

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "FloatMatrix2D.hpp"
/**
 * This class implements segments in the middle of the fluid.
 */

class Segment{
  unsigned int A[2];
  unsigned int B[2]; /* Those points define the segment position in the matrix Actually, 
			we do not need 2*2 int but only three because we enable only 
			vertical and horizontal segments...
		     */
  unsigned int length;
  bool XDirection; //True means the segment in the X-axis direction

public:
  Segment(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
  void setBnd(int, FloatMatrix2D &);

  inline bool isInSegment(unsigned int i, unsigned int j){
    if(XDirection)
      return (A[0]-1 <= i) && (i <= B[0]+1) && (A[1]-1 <= j) && (j <= B[1]+length+1);
    else
      return (A[0]-1 <= i) && (i <= B[0]+length+1) && (A[1]-1 <= j) && (j <= B[1]+1);
  }
  int getA0(){return A[0];}
  int getA1(){return A[1];}
  int getB0(){return B[0];}
  int getB1(){return B[1];}
  int getLength(){return length;}
};

#endif
