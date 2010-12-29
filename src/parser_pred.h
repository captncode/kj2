#pragma once

struct OnExec0
{
  enum { COUNT = 0,};
  template< class F >
  static int32_t exec( F f, char * w[] )
  {
    f();
    return 0;
  }
  template< class T, class F >
  static int32_t execMethod( T * t, F f, char * w[] )
  {
    ( t->*f )();
    return 0;
  }

  static void printArgs() {
    //puts("");
  }
};

template<class A1>
struct OnExec1
{
  enum { COUNT = 1,};
  template< class F>
  static int32_t exec( F f, char * w[] )
  {
    A1 a1;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;

    if( result != COUNT )
    {
      return -COUNT;
    }

    f( a1 );
    return 0;
  }
  template <class T, class F>
  static int32_t execMethod( T * t, F f, char * w[] )
  {
    A1 a1;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;

    if( result != COUNT )
    {
      return -COUNT;
    }

    ( t->*f )( a1 );
    return 0;
  }

  static void printArgs() {
    printf( "%s", getTypeName<A1>() );
  }
};

template< class A1, class A2>
struct OnExec2
{
  enum { COUNT = 2,};
  template< class F >
  static int32_t exec( F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    if( result != COUNT )
    {
      return -COUNT;
    }

    f( a1, a2 );
    return 0;
  }
  template<class T, class F >
  static int32_t execMethod( T * t, F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    if( result != COUNT )
    {
      return -COUNT;
    }

    ( t->*f )( a1, a2 );
    return 0;
  }
  static void printArgs() {
    printf( "%s %s",
            getTypeName<A1>(),
            getTypeName<A2>()
          );
  }
};
template<class A1, class A2, class A3>
struct OnExec3
{
  enum { COUNT = 3,};
  template< class F >
  static void exec( F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    if( result != COUNT )
    {
      return -COUNT;
    }

    f( a1, a2, a3 );
    return 0;
  }
  template< class T, class F >
  static void execMethod( T * t, F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    int32_t result = conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    if( result != COUNT )
    {
      return -COUNT;
    }

    ( t->*f )( a1, a2, a3 );
    return 0;
  }

  static void printArgs() {
    printf( "%s %s %s",
            getTypeName<A1>(),
            getTypeName<A2>(),
            getTypeName<A3>()
          );
  }
};
template<class A1, class A2, class A3, class A4>
struct OnExec4
{
  enum { COUNT = 4,};
  template< class F >
  static int32_t exec( F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    int32_t result = 0;
    result += conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    result += conversionToBuiltin( w[4], &a4 ) ;
    if( result != COUNT )
    {
      return -4;
    }

    f( a1, a2, a3, a4 );
    return 0;
  }

  template< class T, class F >
  static int32_t exec( T * t, F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    int32_t result = 0;
    result += conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    result += conversionToBuiltin( w[4], &a4 ) ;
    if( result != COUNT )
    {
      return -4;
    }

    ( t->*f )( a1, a2, a3, a4 );
    return 0;
  }

  static void printArgs() {
    printf( "%s %s %s %s",
            getTypeName<A1>(),
            getTypeName<A2>(),
            getTypeName<A3>(),
            getTypeName<A4>()
          );
  }

};


template<class A1, class A2, class A3, class A4, class A5>
struct OnExec5
{
  enum { COUNT = 5,};
  template< class F >
  static int32_t exec( F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    A5 a5;
    int32_t result = 0;
    result += conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    result += conversionToBuiltin( w[4], &a4 ) ;
    result += conversionToBuiltin( w[5], &a5 ) ;
    if( result != COUNT )
    {
      return -5;
    }

    f( a1, a2, a3, a4 ,a5 );
    return 0;
  }

  template< class T, class F >
  static int32_t exec( T * t, F f, char * w[] )
  {
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    A5 a5;
    int32_t result = 0;
    result += conversionToBuiltin( w[1], &a1 ) ;
    result += conversionToBuiltin( w[2], &a2 ) ;
    result += conversionToBuiltin( w[3], &a3 ) ;
    result += conversionToBuiltin( w[4], &a4 ) ;
    result += conversionToBuiltin( w[5], &a5 ) ;
    if( result != COUNT )
    {
      return -5;
    }

    ( t->*f )( a1, a2, a3, a4 ,a5 );
    return 0;
  }

  static void printArgs() {
    printf( "%s %s %s %s %s",
            getTypeName<A1>(),
            getTypeName<A2>(),
            getTypeName<A3>(),
            getTypeName<A4>(),
            getTypeName<A5>()
          );
  }

};
