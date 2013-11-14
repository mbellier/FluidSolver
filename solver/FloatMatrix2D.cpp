#include "FloatMatrix2D.hpp"


FloatMatrix2D::FloatMatrix2D(const unsigned int i, const unsigned int j) : 
_width(i), _height(j), _length(i*j)
{
  _values = new float[_length];
  this->fill(0);
}

FloatMatrix2D::FloatMatrix2D(const FloatMatrix2D &m) : 
_width(m._width), _height(m._height), _length(m._length)
{
  std::cerr << "copy\n" << std::endl;
  _values = new float[_length];
  for (unsigned int k = 0; k < _length; k++)
    _values[k] = m._values[k];
}


FloatMatrix2D::~FloatMatrix2D(){
  delete[] _values;
}

void FloatMatrix2D::fill(float v){
  for (unsigned int k=0; k < _length; k++)
    _values[k]=v;
}

void FloatMatrix2D::add(float v){
  for (unsigned int k = 0; k < _length; k++)
    _values[k] += v;
}

void FloatMatrix2D::add(const FloatMatrix2D &m){
  for (unsigned int k = 0; k < _length; k++)
    _values[k] += m._values[k];
}

void FloatMatrix2D::multiplyBy(float v){
  for (unsigned int k = 0; k < _length; k++)
    _values[k] *= v;
}

void FloatMatrix2D::addAndMultiply(const FloatMatrix2D &add, float v){
  for (unsigned int k = 0; k < _length; k++)
    _values[k] += add._values[k] * v;
}

std::ostream &operator<< (std::ostream &stream, const FloatMatrix2D &toPrint){
  for(unsigned int i = 0; i < toPrint.getSize(0); i++){
    for(unsigned int j = 0; j < toPrint.getSize(1); j++)
      stream << std::fixed << std::setprecision(4) << toPrint.get(i,j) << "c";
    stream << "b";
  }
  stream << "a";
  return stream;
}


void FloatMatrix2D::load(const char *file){
  std::ifstream input;
  input.open(file, std::ios::in | std::ios::binary);

  unsigned int n;
  unsigned int l;
  input.read((char *) &n, sizeof(unsigned int));
  input.read((char *) &l, sizeof(unsigned int));
  if (n != 2 || l != _length){
    // TODO: exception
    input.close();
    return;
  }

  input.read((char *) _values, sizeof(float) * _length);
  input.close();
}


void FloatMatrix2D::save(const char *file) const{
  std::ofstream output;
  output.open(file, std::ios::out | std::ios::binary);

  const unsigned int n = 2;
  const unsigned int l = _length;
  output.write((char *) &n, sizeof(unsigned int));
  output.write((char *) &l, sizeof(unsigned int));

  output.write((char *) _values, sizeof(float) * _length);

  output.close();
}
