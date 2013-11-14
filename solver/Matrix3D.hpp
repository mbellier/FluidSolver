#ifndef MATRIX3D_H_
#define MATRIX3D_H_

#include <iostream>
#include <iomanip> // setprecision
#include "Matrix.hpp"

// Ne pas modifier !!!
#define DIMENSION 3

// Class definition
template <typename T>
class Matrix3D : Matrix<T, DIMENSION> {
public:
  Matrix3D(const unsigned int size[N]);
  Matrix3D(const Matrix3D<T> &toCopy);

  inline unsigned int toIndex(const unsigned int i, const unsigned int j, const unsigned int k) const;
  inline T& operator() (const unsigned int i, const unsigned int j, const unsigned int k);        // setter
  inline T operator() (const unsigned int i, const unsigned int j, const unsigned int k) const;   // getter
};

template<typename T>
Matrix3D<T>::Matrix3D(const unsigned int size[DIMENSION]): Matrix<T, DIMENSION>(size){
}

template<typename T>
Matrix3D<T>::Matrix3D(const Matrix3D<T> &toCopy): Matrix<T, DIMENSION>(size){
}

template<typename T>
unsigned int Matrix3D<T>::toIndex(const unsigned int i, const unsigned int j, const unsigned int k) const{
  return i*m_size[1] + j*m_size[1]*m_size[2] + k;
}

template<typename T>
T &Matrix3D<T>::operator() (const unsigned int i, const unsigned int j, const unsigned int k) const{
  return m_values[toIndex(i, j)];
}

template<typename T>
T Matrix3D<T>::operator() (const unsigned int i, const unsigned int j, const unsigned int k) const{
  return m_values[toIndex(i, j)];
}

