#pragma once

/**
  \brief Dynamiczna tablica dwuwymiarowa.

  \author boo
  \date   5.I.2011
  \version 1.0.0
*/

FRAME_NS_BEGIN

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

/*! sprawdzanie zakresu tablicy
    nie uzywałem nawet i nietestowałem
*/
template <typename T>
class Array2D<T, true>
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
    if( !pData ) {
      FRAME_ERROR;
      puts( " array dont exist - create first \n" );
      int error;
      return error;
    }
    if( x >= width ) {
      FRAME_ERROR;
      puts( "array out of bounds\n" );
      int error;
      return error; //  <---zwracanie referenci do zmiennej lokalnej
    }
    if( y >= height ) {
      FRAME_ERROR;
      puts( "array out of bounds\n" );
      int error;
      return error;;
    }
    return pData[x * height + y];
  }

  T * operator*( int x ) {
    if( !pData ) {
      FRAME_ERROR;
      puts( " array dont exist\n" );
      return 0;
    }
    if( x >= width ) {
      FRAME_ERROR;
      puts( "array out of bounds\n" );
      return 0;
    }
    return pData[x*height];
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


FRAME_NS_END
