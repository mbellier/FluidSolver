#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <fstream>
#include <iomanip> // setprecision

// Necessary declarations for friendly fonctions
template <typename T, unsigned int N>
class Matrix;

// Some operators
template<typename T, unsigned int N>
Matrix<T, N> &operator+ (const Matrix<T, N> &a, const Matrix<T, N> &b);
template<typename T, unsigned int N>
Matrix<T, N> &operator- (const Matrix<T, N> &a, const Matrix<T, N> &b);
template<typename T, unsigned int N>
std::ostream &operator<<  (std::ostream &stream, const Matrix<T, N> &toPrint);

// Class definition
template <typename T, unsigned int N>
class Matrix {
protected:
  T *m_values;
  unsigned int m_size[N];
  unsigned int m_length;
  
  unsigned int toIndex(const unsigned int coordinates[N]) const;
  virtual unsigned int *toCoordinates(const unsigned int &index) const; 
  bool checkCoordinates(const unsigned int coordinates[N]) const;
  bool checkSizes(const Matrix<T, N> &m) const;
  
public:
  Matrix(const unsigned int size[N]);
  Matrix(const Matrix<T, N> &toCopy);
  virtual ~Matrix();
  
  unsigned int *getSize() const;
  unsigned int getSize(unsigned int dimension) const;

  void fill(const T &value);

  void load(char *file);
  void save(char *file) const;

  Matrix<T, N> &operator= (const Matrix<T, N> &toCopy);
  T& operator() (const unsigned int coordinates[N]);        // setter
  T operator() (const unsigned int coordinates[N]) const;   // getter

  // Operators .= with another matrix :
  Matrix<T, N> &operator+= (const Matrix<T, N> &toAdd);
  Matrix<T, N> &operator-= (const Matrix<T, N> &toSub);
  
  // Operators .= with a scalar :
  Matrix<T, N> &operator+= (const T &toAdd);
  Matrix<T, N> &operator-= (const T &toSub);
  Matrix<T, N> &operator*= (const T &toMul);
  Matrix<T, N> &operator/= (const T &toDiv);

  // Basic operations with a scalar :
  Matrix<T, N> &operator+ (const T &toAdd);
  Matrix<T, N> &operator- (const T &toSub);
  Matrix<T, N> &operator* (const T &toMul);
  Matrix<T, N> &operator/ (const T &toDiv);

  // Addition and substraction between martices :
  friend Matrix<T, N> &(::operator+ <>) (const Matrix<T, N> &a, const Matrix<T, N> &b);
  friend Matrix<T, N> &(::operator- <>) (const Matrix<T, N> &a, const Matrix<T, N> &b);

  
  friend std::ostream &(::operator<< <>) (std::ostream &stream, const Matrix<T, N> &toPrint);
};


/********************************************************************/
/*                 Implementation of private methods                   */
/********************************************************************/
template<typename T, unsigned int N>
unsigned int Matrix<T, N>::toIndex(const unsigned int coordinates[N]) const{
  unsigned int index(0);
  
  for(unsigned int i(0); i < N - 1; i++)
    index = (index + coordinates[i])*m_size[i + 1];
  
  return index + coordinates[N - 1];
}

template<typename T, unsigned int N>
unsigned int *Matrix<T, N>::toCoordinates(const unsigned int &index) const{
  unsigned int *coordinates = new unsigned int[N];
  unsigned int tmp_index = index, tmp_prod = 1;
  
  for(unsigned int i(1); i < N; i++)
    tmp_prod *= m_size[i];
  
  for(unsigned int i(1); i < N; i++){
    coordinates[i - 1] = tmp_index/tmp_prod;
    tmp_index %= tmp_prod;
    tmp_prod /= m_size[i];
  }
  
  return coordinates;
}

template<typename T, unsigned int N>
bool Matrix<T, N>::checkCoordinates(const unsigned int coordinates[N]) const{
  for(int i(0); i < N; i++)
    if(coordinates[i] < 0 || m_size[i] <= coordinates[i])
      return false;
  
  return true;
}

template<typename T, unsigned int N>
bool Matrix<T, N>::checkSizes(const Matrix<T, N> &m) const{
  for(unsigned int i(0); i < m_length; i++)
    if(m.m_size[i] != m_size[i])
      return false;
  
  return true;
}



/********************************************************************/
/*               Implementation of public methods                   */
/********************************************************************/
template<typename T, unsigned int N>
Matrix<T, N>::Matrix(const unsigned int size[N]): m_length(1){
  for(unsigned int i(0); i < N; i++){
    m_size[i] = size[i];
    m_length *= size[i];
  }

  m_values = new T[m_length];
  for(unsigned int i(0); i < m_length; i++){
    m_values[i] = 0;
  }
}

template<typename T, unsigned int N>
Matrix<T, N>::Matrix(const Matrix<T, N> &toCopy) {
  unsigned int i;
  for(i=0; i < N; i++){
    m_size[i] = toCopy.m_size[i];
  }
  m_length = toCopy.m_length;
  m_values = new T[m_length];
}

// Destructor
template<typename T, unsigned int N>
Matrix<T, N>::~Matrix(){
  delete[](m_values);
}

template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator= (const Matrix<T, N> &toCopy){
  if(this == &toCopy)
    return *this;
  
  // TODO throw exception if toCopy.m_size != this.m_size

  // 1) allocate new memory and copy elements
  T *new_values = new T[toCopy.m_length];
  new_values = toCopy.m_values;
  
  // 2) delete old memory
  delete [] m_values;
  
  // 3) assign the new memory to the object
  m_values = new_values;
  
  return *this;
}

// Setter
template<typename T, unsigned int N>
inline
T& Matrix<T, N>::operator() (const unsigned int coordinates[N]){
  return m_values[toIndex(coordinates)];
}

// Getter
template<typename T, unsigned int N>
inline
T Matrix<T, N>::operator() (const unsigned int coordinates[N]) const{
  return m_values[toIndex(coordinates)];
}


// File IO
template<typename T, unsigned int N>
void Matrix<T, N>::load(char *file){
  std::ifstream input;
  input.open(file, std::ios::in | std::ios::binary);

  unsigned int n;
  unsigned int l;
  input.read((char *) &n, sizeof(unsigned int));
  input.read((char *) &l, sizeof(unsigned int));
  if (n != N || l != m_length){
    // TODO: exception
    input.close();
    return;
  }

  input.read((char *) m_values, sizeof(T) * m_length);
  input.close();
}

template<typename T, unsigned int N>
void Matrix<T, N>::save(char *file) const{
  std::ofstream output;
  output.open(file, std::ios::out | std::ios::binary);

  const unsigned int n = N;
  const unsigned int l = m_length;
  output.write((char *) &n, sizeof(unsigned int));
  output.write((char *) &l, sizeof(unsigned int));

  output.write((char *) m_values, sizeof(T) * m_length);

  output.close();
}


/**
 * Returns the size of the matrix as an array
 */
template<typename T, unsigned int N>
inline
unsigned int *Matrix<T, N>::getSize() const{
  return m_size;
}

/**
 * Returns the size of the matrix for a given dimension
 */
template<typename T, unsigned int N>
inline
unsigned int Matrix<T, N>::getSize(unsigned int dimension) const{
  return m_size[dimension];
}

/**
 * Fills the matrix with a given value.
 */
template<typename T, unsigned int N>
inline
void Matrix<T, N>::fill(const T &value){
  memset(m_values, value, m_length*sizeof(T));
}
  
/********************************************************************/
/*       Implementation of .= operators with another matrix         */
/********************************************************************/
template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator+= (const Matrix<T, N> &toAdd){
  // TODO throw exception if toAdd.m_size != this.m_size
  
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] += toAdd.m_values[i];
  
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator-= (const Matrix<T, N> &toSub){
  // TODO throw exception if toAdd.m_size != this.m_size
  
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] -= toSub.m_values[i];
  
  return *this;
}

/********************************************************************/
/*          Implementation of .= operators with a scalar            */
/********************************************************************/
template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator+= (const T &toAdd){
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] += toAdd;
  
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator-= (const T &toSub){
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] -= toSub;
  
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator*= (const T &toMul){
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] *= toMul;
  
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> &Matrix<T, N>::operator/= (const T &toDiv){
  for(unsigned int i(0); i < m_length; i++)
    m_values[i] /= toDiv;
  
  return *this;
}

/********************************************************************/
/*        Implementation of basic operators with a scalar           */
/********************************************************************/
template<typename T, unsigned int N>
inline
Matrix<T, N> &Matrix<T, N>::operator+ (const T &toAdd){
  return *this += toAdd;
}

template<typename T, unsigned int N>
inline
Matrix<T, N> &Matrix<T, N>::operator- (const T &toSub){
  return *this -= toSub;
}

template<typename T, unsigned int N>
inline
Matrix<T, N> &Matrix<T, N>::operator* (const T &toMul){
  return *this *= toMul;
}

template<typename T, unsigned int N>
inline
Matrix<T, N> &Matrix<T, N>::operator/ (const T &toDiv){
  return *this /= toDiv;
}


/********************************************************************/
/*           Addition and Substraction between matrices             */
/********************************************************************/
template<typename T, unsigned int N>
Matrix<T, N> &operator+ (const Matrix<T, N> &a, const Matrix<T, N> &b){
  Matrix<T, N> copy(a);
  return copy += b;
}

template<typename T, unsigned int N>
Matrix<T, N> &operator- (const Matrix<T, N> &a, const Matrix<T, N> &b){
  Matrix<T, N> copy(a);
  return copy -= b;
}


// Stream operator (only for 2 dimensions matrices)
template<typename T, unsigned int N>
std::ostream &operator<< (std::ostream &stream, const Matrix<T, N> &toPrint){
  if(N != 2){
    stream << "Cette matrice est de dimension " << N << "." << std::endl;
    return stream;
  }
  
  for(  unsigned int coordinates[2] = {0}; coordinates[0] < toPrint.m_size[0]; coordinates[0]++){
    for(coordinates[1] = 0; coordinates[1] < toPrint.m_size[1]; coordinates[1]++)
      stream << std::fixed << std::setprecision(3) << toPrint.m_values[toPrint.toIndex(coordinates)] << " ";
    stream << std::endl;
  }
  
  return stream;
}

#endif /* MATRIX_H_ */
