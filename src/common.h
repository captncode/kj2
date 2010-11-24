#pragma once
#include "precompiled.h"
#include "assert.h"
#include "stdint.h"
#include <cmath>
#include <cstdio>
#include <ctime>
#include <algorithm>

#define PI    (3.14159265f)


#define EPSILONF4    (10e-4)
#define EPSILONF5    (10e-5)
#define EPSILONF6    (10e-6)

//int printf( const char *format, ... );
namespace std {float pow( float, float );}


struct Float3 {
  float x, y, z;
};

struct Float2 {
  float x, y;
};

struct Int2 {
  int x, y;
};

struct Int3 {
  int x, y, z;
};

template <typename T>
struct XYZ;

template <typename T>
struct XY {
  typedef XY<T> MyType;
  T x, y;

  XY(): x(), y()
  {}
  XY( const T & aX, const T & aY ): x( aX ), y( aY )
  {}
  explicit XY( const T & aXY ): x( aXY ), y( aXY )
  {}
  template<class W>
  XY( const W & v ): x( v.x ), y( v.y )
  {}

  MyType & operator=( const MyType & r ) {
    this->x = r.x;
    this->y = r.y;
    return *this;
  }
  template<class W>
  MyType & operator=( const XYZ<W>& r );

  MyType operator+( const MyType & r ) const {
    return MyType( x + r.x, y + r.y );
  }

  MyType & operator+=( const MyType & r ) {
    x += r.x;
    y += r.y;
    return *this;
  }

  MyType operator+( ) const {
    return *this;
  }

  MyType operator-( const MyType & r )   const {
    return MyType( x - r.x, y - r.y );
  }
  MyType & operator-=( const MyType & r ) {
    x -= r.x;
    y -= r.y;
    return *this;
  }
  MyType operator-( ) const {
    return MyType( -x, -y );
  }

  MyType operator*( const T & r ) const {
    return MyType( x * r , y * r );
  }
  const MyType & operator*=( const T & r ) {
    x *= r;
    y *= r;
    return *this;
  }

  MyType operator/( const T & r ) const {
    if( !r )  return *this;
    return MyType( x / r , y / r );
  }
  const MyType & operator/=( const T & r ) {
    if( !r )  return *this;
    x /= r;
    y /= r;
    return *this;
  }

  template<class W>
  bool operator== ( const XY<W>& r ) const {
    if( cmpf( r.x, x ) && cmpf( r.y, y ) )
      return true;
    return false;
  }

  template<class W>
  bool operator== ( const XYZ<W>& r ) const ;

  template<class W>
  operator XY<W> () const {
    return XY<W>( W( x ), W( y ) );
  }
};
typedef XY<float> Vec2;


template <typename T>
inline
T dot( const XY<T>& l, const XY<T>&r )
{
  return l.x * r.x + l.y * r.y;
}

template <typename T>
inline
XY<T> scale( const XY<T>& l, const XY<T>&r )
{
  return XY<T>( l.x * r.x, l.y * r.y );
}

template <typename T>
inline
T length( const XY<T>& wh )
{
  return pow(( pow( wh.x, 2 ) + pow( wh.y, 2 ) ), 0.5f );
}

template <typename T>
inline
XY<T> normalize( const XY<T>& wh )
{
  return wh / length( wh );
}

template <typename T>
struct XYZ {
  typedef XYZ<T> MyType;
  T x, y, z;

  XYZ(): x(), y(), z()
  {}
  XYZ( const T & aX, const T & aY, const T & aZ ): x( aX ), y( aY ), z( aZ )
  {}
  explicit XYZ( const XY<T>& a ): x( a.x ), y( a.y ), z()
  {}
  explicit XYZ( const T & aXYZ ): x( aXYZ ), y( aXYZ ), z( aXYZ )
  {}
  template < class W >
  explicit XYZ( const W & v ): x( v.x ), y( v.y ), z( v.y )
  {}

//    //! uwaga, da sie rzutować na weszystko co ma konstruktor 3 argumentowy
//    template <class W>
//    operator W (){
//        return W(x,y,z);
//    }

  MyType & operator=( const MyType & r ) {
    this->x = r.x;
    this->y = r.y;
    this->z = r.z;
    return *this;
  }

  template<class W>
  MyType & operator= ( const XY<W>& r ) {
    x = r.x;
    y = r.y;
    z = W();
    return *this;
  }

  MyType operator+( const MyType & r ) const {
    return MyType( x + r.x, y + r.y, z + r.z );
  }
  MyType & operator+=( const MyType & r ) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
  }
  MyType operator+( ) const {
    return *this;
  }

  MyType operator-( const MyType & r ) const {
    return MyType( x - r.x, y - r.y, z - r.z );
  }
  MyType & operator-=( const MyType & r ) {
    x -= r.x;
    y -= r.y;
    z -= r.z;
    return *this;
  }
  MyType operator-( ) const {
    return MyType( -x, -y, -z );
  }

  MyType operator*( const T & r ) const {
    return MyType( x * r , y * r, z * r );
  }
  const MyType & operator*=( const T & r ) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }

  MyType operator/( const T & r ) const {
    if( !r )return *this;
    return MyType( x / r, y / r, z / r );
  }
  const MyType & operator/=( const T & r ) {
    if( !r )return *this;
    x /= r;
    y /= r;
    z /= r;
    return *this;
  }


  template<class W>
  bool operator== ( const XYZ<W>& r ) const {
    if( cmpf( r.x , x ) && cmpf( r.y, y ) && cmpf( r.z, z ) )
      return true;
    return false;
  }

  template<class W>
  bool operator== ( const XY<W>& r ) const {
    if( cmpf( W(), z ) && cmpf( r.x , x ) && cmpf( r.y , y ) )
      return true;
    return false;
  }

  template<class W>
  operator XYZ<W> () const {
    return XYZ<W>( W( x ), W( y ), W( z ) );
  }
};
typedef XYZ<float> Vec3;

template<class T>
template<class W>
bool XY<T>::operator== ( const XYZ<W>& r ) const
{
  if( cmpf( T(), r.z ) && cmpf( r.x, x ) && cmpf( r.y, y ) )
    return true;
  return false;
}

template<class T>
template<class W>
XY<T>& XY<T>::operator=( const XYZ<W>& r )
{
  x = r.x;
  y = r.y;
  return *this;
}

template <typename T>
inline
T dot( const XYZ<T>& l, const XYZ<T>&r )
{
  return l.x * r.x + l.y * r.y + l.z * r.z;
}

template <typename T>
inline
T length( const XYZ<T>& wh )
{
  return pow(( pow( wh.x, 2 ) + pow( wh.y, 2 ) + pow( wh.y, 2 ) ), 0.5f );
}

template <typename T>
inline
XYZ<T> normalize( const XYZ<T>& wh )
{
  return wh / length( wh );
}

/*
inline
template <typename T>
XYZ<T> cross(const XYZ<T>& l, const XYZ<T>&r )
{
    return {};
}
*/


template <typename T>
inline
bool isBetween( const T & what, const T & down, const T & up )
{
  const T * pDown = &down;
  const T * pUp = &up;
  if( down > up ) {
    pDown = &up;
    pUp = &down;
  }
  if( what > *pDown && what < *pUp )
    return true;
  return false;

}

template <typename T>
inline
bool isBetweenEq( const T & what, const T & down, const T & up )
{
  const T * pDown = &down;
  const T * pUp = &up;
  if( down > up ) {
    pDown = &up;
    pUp = &down;
  }
  if( what >= *pDown && what <= *pUp )
    return true;
  return false;
}

template <typename T>
inline
T clamp( const T & what, const T & down, const T & up )
{
  return std::min( up, std::max( what, down ) );
}

/*!  w której ćwiartce jest kąt, pamiętać aby dawać parametry w zakresie <-1,1>
        pierwszy podawany jest cosinus odpowiadający współrzędnej x znormalizowanego wektora
        -1 - (1,0)  - 0 st
        -2 - (0,1) - 90 st
        -3 - (-1,0)  -180 st
        -4 - (0,-1) -270 st

*/
inline
int8_t angleWithQuarter( float aCos, float aSin )
{

  if( aSin > 0 ) {
    if( aCos > 0 )
      return 1;
    else if( aCos < 0 )
      return 2;
    else    //sin = 1, cos = 0 => (0,1)
      return -2;
  }
  else if( aSin < 0 ) {
    if( aCos > 0 )
      return 4;
    else if( aCos < 0 )
      return 3;
    else    //sin = -1, cos =0 => (0,-1)
      return -4;
  }
  else {  //sin == 0
    if( aCos > 0 )  //sin = 0, cos = 1 => (1,0)
      return -1;
    else if( aCos < 0 ) //sin = 0, cos = -1 => (-1,0)
      return -3;
    else    //wtf
      return 0;
  }

}

template <typename T>
inline
char angleWithQuarter( const XY<T>& v )
{
  const XY<T> v2( normalize( v ) );
  return angleWithQuarter( v2.x, v2.y );
}

template <typename T>
inline
int8_t sign( const T & t )
{
  if( !t )     return 0;
  return ( t > 0 ) ? 1 : -1;
}

template <typename T>
inline
T aritmeticAverange( const T * begin, const T * end )
{
  T count = 0, sum = 0;

  for( const T * it = begin; it != end; ++it, ++count ) {
    sum += *it;
  }
  return sum / count;
}

//! obraca wektor o kąt skierowany radians
inline
Vec2 rotate( const Vec2 & v, float radians )
{
  const float sr = sin( radians );
  const float cr = cos( radians );
  return Vec2( v.x * cr - v.y * sr , v.x * sr + v.y * cr );
}

//! ustawia kąt nachylenia do osi OX
inline
Vec2 inclineToOXBy( const Vec2 & v, float radians )
{
  return Vec2( cos( radians ), sin( radians ) ) * length( v );
}

/*! Porównuje floaty uwzględniając niedokładność
*/
inline
bool cmpf( float v1, float v2 )
{
  return std::abs( v1 - v2 ) < EPSILONF5;

}

template<typename T, size_t I>
size_t getStaticStringLength( T( &n )[I] )
{
  return I / sizeof( T );
}

//jest ot samo w static_for.h
//template <uint32_t L,uint32_t R>
//struct Max
//{
//    enum{
//        RETURN = L>R?L:R,
//    };
//};
//
template <uint32_t L, uint32_t R>
struct Min {
  enum {
    RETURN = L > R ? R : L,
  };
};

template<size_t N>
struct CString {
  enum {
    CAPACITY = N,
  };
  CString() {
    memset( str, 0, N );
  }
  template< uint32_t N_>
  CString( char( &str_ )[N_] ) {
    //memset( str,0,N );
    strncpy( str, str_, Min<N, N_>::RETURN );
  }
  CString( const char * str_ ) {
    //memset( str,0,N );

    // z dokumentacji:
    //If the end of the source C string (which is signaled by a null-character)
    //is found before num characters have been copied, destination is padded
    //with zeros until a total of num characters have been written to it.
    strncpy( str, str_, N );
  }
  operator char*() {
    return str;
  }
  operator const char*() const {
    return str;
  }
  /*! konstruktor wymaga, aby ta tablica byla mutowalna
  */
  mutable char str[N];
};

uint32_t loadFileToBuffer( const char * name, char ** pBuffer, size_t * pOutSize );

inline void sleepLoop ( int milis )
{
  clock_t a;
  a = clock () + milis ;
  while (clock() < a) {}
}

inline uint32_t makeRGBA(char r,char g,char b,char a){
  return r<<24 | g<<16 | b<<8 | a;
}
inline uint32_t MakeRGBX(char r,char g,char b){
  return r<<24 | g<<16 | b<<8 | 0xff;
}

