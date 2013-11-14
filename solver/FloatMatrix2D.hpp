#ifndef FLOATMATRIX2D_HPP_
#define FLOATMATRIX2D_HPP_

#include <iostream>
#include <fstream>
#include <iomanip> // setprecision

/**
 * This class implements 2D matrices of floats
 *
 */

class FloatMatrix2D {
private:
  float *_values;
  const unsigned int _width, _height;
  const unsigned int _length;
public:
  FloatMatrix2D(const unsigned int i, const unsigned int j);
  FloatMatrix2D(const FloatMatrix2D &m);
  ~FloatMatrix2D();

  inline float get(const unsigned int i, const unsigned int j) const{
    return _values[j * _width + i];
  }

  inline void set(const unsigned int i, const unsigned int j, const float value){
    _values[j * _width + i] = value;
  }

  inline float *getArray(){
    return _values;
  }

  inline unsigned int getSize(const unsigned int dim) const{
    return (dim) ? _width : _height;
  }

  inline unsigned int getLength() const {
    return _length; 
  }

  void fill(float v);

  
  void add(float v);
  void add(const FloatMatrix2D &m);
  inline void multiplyBy(float v);
  void addAndMultiply(const FloatMatrix2D &add, float v);

  void load(const char *file);
  void save(const char *file) const;

};

std::ostream &operator<< (std::ostream &stream, const FloatMatrix2D &toPrint);

#endif
