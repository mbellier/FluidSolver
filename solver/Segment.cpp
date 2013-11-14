#include "Segment.hpp"

Segment::Segment(unsigned int N_x, unsigned int N_y, unsigned int A0, unsigned int A1, unsigned int B0, unsigned int B1, unsigned int L0){
  length = L0;
  if(A0 >= N_x-1 || A0 <= 1
     || A1 >= N_y-1 || A1 <= 1
     || B0 >= N_x-1 || B0 <= 1
     || B1 >= N_y-1 || B1 <= 1
     || (A0 != B0 && A1 != B1)){  // FUTURE : ERROR MANAGER ?
    fprintf(stderr, "Bad index for Segment Localisation\n");
    fprintf(stderr, "(%d,%d)-(%d,%d) not accepted\n", A0, A1, B0, B1);
    exit(EXIT_FAILURE);
  }

  bool AIsMin;
  // Basic computations
  XDirection = (A1 == B1);
  if(XDirection)
    AIsMin = (A0 <  B0);
  else
    AIsMin = (A1 <  B1);

  if(AIsMin){
    A[0] = A0;
    A[1] = A1;
    B[0] = B0;
    B[1] = B1;
  }
  else{
    A[0] = B0;
    A[1] = B1;
    B[0] = A0;
    B[1] = A1;
  }

  // Error length
  if((XDirection && !(B[1]+length < N_y-2))
     ||(!XDirection && !(B[0]+length < N_x-2))){
    fprintf(stderr, "Bad index for Segment Localisation\n");
    fprintf(stderr, "(%d,%d)-(%d,%d) not accepted with length %d\n", A0, A1, B0, B1, L0);
    exit(EXIT_FAILURE);
  }
}

/**
 * Defines the behaviour around the obstacle 
 *  
 * @param b Defines the border behaviour
 * @param x Matrix to modify
 */
void Segment::setBnd(int b, FloatMatrix2D &x){
  unsigned int i = 0;
  if(XDirection){
    for (i = A[0]; i <= B[0]; i++){
      x.set(i, A[1]+length+1, b==2 ? -x.get(i, A[1]+length+2) : x.get(i, A[1]+length+2));
      for(unsigned int j = A[1]; j < A[1]+length; j++)
	x.set(i, j, 0);
      x.set(i, A[1]-1, b==2 ? -x.get(i, A[1]-2) : x.get(i, A[1]-2));
    }
    // Extremities :
    for(i = A[1]; i <= A[1]+length; i++){
      x.set(A[0]-1, i, b==1 ? -x.get(A[0]-2, i) : x.get(A[0]-2, i));
      x.set(B[0]+1, i, b==1 ? -x.get(B[0]+2, i) : x.get(B[0]+2, i));
    }
    // Angles : this method loses a minimum of matter - but not perfect
    x.set(A[0]-1, A[1]+length+1, 
    0.5f*(x.get(A[0]-1,A[1]+length) + x.get(A[0],A[1]+length+1)));
    x.set(A[0]-1, A[1]-1,
    0.5f*(x.get(A[0]-1,A[1]) + x.get(A[0],A[1]-1)));
    x.set(B[0]+1, B[1]+length+1, 
    0.5f*(x.get(B[0]+1,B[1]+length) + x.get(B[0],B[1]+length+1)));
    x.set(B[0]+1, B[1]-1,
    0.5f*(x.get(B[0]+1,B[1]) + x.get(B[0],B[1]-1)));
  }
  else{
    for (i = A[1]; i <= B[1]; i++){
      x.set(A[0]+length+1, i, b==1 ? -x.get(A[0]+length+2, i) : x.get(A[0]+length+2, i));
      for(unsigned int j = A[0]; j <= A[0]+length; j++)
	x.set(j, i, 0);
      x.set(A[0]-1, i, b==1 ? -x.get(A[0]-2, i) : x.get(A[0]-2, i));
    }
    // Extremities :
    for(i = A[0]; i <= A[0]+length; i++){
      x.set(i, A[1]-1, b==2 ? -x.get(i, A[1]-2) : x.get(i, A[1]-2));
      x.set(i, B[1]+1, b==2 ? -x.get(i, B[1]+2) : x.get(i, B[1]+2));
    }
    // Angles : this method loses a minimum of matter - but not perfect
    x.set(A[0]+length+1, A[1]-1,
    0.5f*(x.get(A[0]+length,A[1]-1) + x.get(A[0]+length+1,A[1])));
    x.set(A[0]-1, A[1]-1,
    0.5f*(x.get(A[0],A[1]-1) + x.get(A[0]-1,A[1])));
    x.set(B[0]+length+1, B[1]+1,
    0.5f*(x.get(B[0]+length,B[1]+1) + x.get(B[0]+length+1,B[1])));
    x.set(B[0]-1, B[1]+1,
    0.5f*(x.get(B[0],B[1]+1) + x.get(B[0]-1,B[1])));
  }
}
