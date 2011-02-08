#pragma once

/**
  \brief Dynamiczna tablica dwuwymiarowa.

  \author boo
  \date   5.I.2011
  \version 1.0.0
*/



template<class T,uint32_t N>
struct Array1D{
  Array1D (T arr_[N] ){
    for(int i = 0; i < (int)N; ++i ){
      arr[i] = arr_[i];
    }/*koniec for (i)*/
  }
  T arr[N];
  T& operator[] (int i){
    return arr[i];
  }
  T* operator* (){
    return *arr;
  }

};

/*! opakowanie na dunamiczna tablice dwuwymiarową,
    Array2D tabIntTest(20,10);
    tabIntTest[5][8] = 87654;
    assert( tabIntTest(5,8) == 87654 );
*/
template < typename T, bool bound_check = false >
class Array2D
{
public:
  Array2D() : pData( 0 ), width( 0 ), height( 0 ) {
  }
  Array2D( int _width, int _height ) : width( _width ), height( _height ) {
    pData = new T[width * height];
  }
  Array2D( const Array2D & r ): width( r.width ), height( r.height ) {
    pData = new T[width * height];
    memcpy( pData, r.pData, width * height );
  }
  const Array2D & operator= ( const Array2D & r ) {
    create( r.width, r.height );
    memcpy( pData , r.pData, width * height );
    return *this;
  }
  ~Array2D() {
    delete pData;
    pData = 0;
  }

  T & create( int _width, int _height ) {
    width = _width;     //x
    height = _height;   //y
    delete pData;
    pData = new T[width * height];
    return pData[0];
  }

  T & operator()( int x, int y ) {
    return pData[x * height + y];
  }
  T * operator[]( int x ) {
    return &pData[x*height];//zrwaca kolumne, troche to zjebałem
  }

  T operator()( int x, int y ) const {
    return pData[x * height + y];
  }
  const T * operator[]( int x )        const {
    return &pData[x*height];
  }

  uint32_t getWidth()const {
    return width;
  }
  uint32_t getHeight()const {
    return height;
  }

  uint32_t getLength()const {
    return height * width;
  }

  T * begin()const {
    return pData;
  }

private:
  T * pData;
  int width, height;
};

