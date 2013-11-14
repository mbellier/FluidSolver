#ifndef MATRIX2D_H_
#define MATRIX2D_H_

#include <iostream>
#include <iomanip> // setprecision
#include "Matrix.hpp"

// Ne pas modifier !!!
#define DIMENSION 2

// Class definition
template <typename T>
class Matrix2D : public Matrix<T, DIMENSION> {
public:
  Matrix2D(const unsigned int size[DIMENSION]);
  Matrix2D(const Matrix2D<T> &toCopy);

  inline unsigned int toIndex(const unsigned int i, const unsigned int j) const;
  inline T& operator() (const unsigned int i, const unsigned int j);        // setter
  inline T operator() (const unsigned int i, const unsigned int j) const;   // getter
};

template<typename T>
Matrix2D<T>::Matrix2D(const unsigned int size[DIMENSION]): Matrix<T, DIMENSION>(size){
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D<T> &toCopy): Matrix<T, DIMENSION>(toCopy.m_size){
}

template<typename T>
unsigned int Matrix2D<T>::toIndex(const unsigned int i, const unsigned int j) const{
  return i * this->m_size[1] + j;
}

template<typename T>
T &Matrix2D<T>::operator() (const unsigned int i, const unsigned int j){
  return this->m_values[toIndex(i, j)];
}

template<typename T>
T Matrix2D<T>::operator() (const unsigned int i, const unsigned int j) const{
  return this->m_values[toIndex(i, j)];
}

#endif
