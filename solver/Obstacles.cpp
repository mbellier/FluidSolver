#include "Obstacles.hpp"
#include <list>

Obstacles::Obstacles(unsigned int N_x, unsigned int N_y, Config &config) : _N_x(N_x), _N_y(N_y){
  // Reads Segments from XML file
  int ** res = config.getSegmentValues();
  unsigned int nbSegments = config.getSegmentNb();

  // Instantiates Segments
  for(unsigned int i = 0; i < nbSegments; i++)
    segList.push_front(new Segment(N_x, N_y, res[i][0], res[i][1], res[i][2], res[i][3], res[i][4]));
}

Obstacles::Obstacles(){}

Obstacles::~Obstacles(){
  reset(); // clear the list
}

void Obstacles::setObstacles(int b, FloatMatrix2D &x){
  std::list<Segment*>::iterator iter;
  for(iter = segList.begin(); iter != segList.end(); iter++)
    (*iter)->setBnd(b, x);
}

void Obstacles::addSegment(unsigned int A0, unsigned int A1, \
  unsigned int B0, unsigned int B1, unsigned int L0){
  segList.push_front(new Segment(_N_x, _N_y, A0, A1, B0, B1, L0));
}

void Obstacles::reset(){
  std::list<Segment*>::iterator iter;
  for(iter = segList.begin(); iter != segList.end(); iter++){
    delete (*iter);
  }
  segList.clear();
}
