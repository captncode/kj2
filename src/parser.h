#pragma once
#include "common.h"
#include "parser_pred.h"
#include "big_pointer.h"
#include <cassert>

namespace parserNs {

namespace nullPrint {
inline void printf( const char * , ... ) {}
inline void puts( const char *  ) {}
}

namespace writer = std;
//namespace writer = nullPrint;

struct BaseParserItem
{
  template<class T>
  BaseParserItem( T * t ) : bp( t ) {};
  template<class T>
  BaseParserItem( T t ) : bp( t ) {};
  ~BaseParserItem() {};

  virtual void execute( char * w[], uint32_t size ) {
    (void) sizeof(w);
    (void) sizeof(size);
  }
  virtual void executeMethod( char * w[], uint32_t size , BigPointer ptr ) {
    (void) sizeof(w);
    (void) sizeof(size);
    (void) sizeof(ptr);
  };

  template<class T> T getPointer() const {
    return static_cast<T>( bp );
  }

private:
  BigPointer bp;
}; //koniec BaseParserItem

template < class T>
class Variable : public BaseParserItem
{
public:
  Variable( T * t ) : BaseParserItem( t ), v(( T * )bp ) {
  }
  ~Variable() {
  };

  virtual void execute( char * w[], uint32_t size )
  {
    T t;
    int r = conversionToBuiltin( w[1], &t );
    if( !r ) {
      //błąd
      return;
    }
    *v = t;

  }
private:
  T * v;
}; //koniec Variable

template<class OnExecPred, class PF>
class Function : public BaseParserItem
{
public:
  Function( PF f ): BaseParserItem( f ) , func( f ) {}
  ~Function() {}

  virtual void execute( char * w[], uint32_t size )
  {
    if( size < OnExecPred::COUNT ) {
      printf( "Too few arguments to '%s\' try: '%s ", w[0], w[0] );
      OnExecPred::printArgs();
      puts( "'" );
      return;
    }

    if( OnExecPred::exec( func , w )  < 0 ) {
      printf( "Wrong arguments,try: %s '", w[0] );
      OnExecPred::printArgs();
      puts( "'" );
    }
  }

private:
  PF func;
};

template<class OnExecPred, class MP, class T>
class Method : public BaseParserItem
{
public:
  Method( MP mp ) : BaseParserItem( mp ) , func( mp ) {}
  ~Method() {}

  void executeMethod( char * w[], uint32_t size, BigPointer ptr ) {

    if( size < OnExecPred::COUNT ) {
      printf( "Too few arguments to '%s\' try: '%s ", w[0], w[0] );
      OnExecPred::printArgs();
      puts( "'" );
      return;
    }
    if( OnExecPred::execMethod( static_cast<T *>( ptr.ptr ), func , w )  < 0 ) {
      printf( "Wrong arguments,try: %s '", w[0] );
      OnExecPred::printArgs();
      puts( "'" );
    }
  }
protected:
  MP func;
private:
}; /* koniec Method*/



template< class T >
class ParserObject : public BaseParserItem
{
public:
  ParserObject( T * t_ ): BaseParserItem( t_ ), typeName( typeid( T ).name() ),
    t( t_ )
  {
    typeName = typeid( T ).name();
  }

  virtual
  void execute( char * w[], uint32_t size )
  {
    MethodIterator it = methodItem.find( w[0] );
    if( it == methodItem.end() )
      return;

    BaseParserItem * func = it->second;
    func->executeMethod( w, size, t );

  }

  template<class R>
  void addMthd0( R( T::*pm )(), const char * name )
  {
    methodItem.insert( std::make_pair( name,
                                       new Method<OnExec0, R( T:: * )(), T>( pm ) ) );
  }
  template<class R, class A1>
  void addMthd1( R( T::*pm )( A1 ), const char * name )
  {
    methodItem.insert( std::make_pair( name,
                                       new Method < OnExec1<A1>, R( T:: * )( A1 ),
                                       T > ( pm ) ) );
  }
  const char * getTypeName()const  { return typeName.c_str(); }

  typedef std::map<std::string, BaseParserItem * > MethodMap;
  typedef MethodMap::iterator MethodIterator;

  //static zapewnia jedną kopie na typ :)
  static MethodMap methodItem;
  static int dupa;

private:
  std::string typeName;
  T * t;
};

template<class T>
std::map<std::string, BaseParserItem * >
ParserObject<T>::methodItem =
  ParserObject<T>::MethodMap() ;


void setParserVar( const char * varName, const char * value );


class Parser
{
public:
  Parser();
  virtual ~Parser();

  template<typename F>
  static const char * getMask( F f ) {
    return 0;
  }
  int32_t explode( char * what, std::vector<char *>& w ) const ;
  int32_t explode( char * what, std::vector<int>& w ) const ;
  int32_t explode( char * what, std::vector<std::string>& w ) const ;

  void objectNameToPointer( std::vector<std::string>& w2 );
  void objectNameToPointer( std::vector<char *>& w2 );
  void parse( char * what );
  void printAll() ;
  BaseParserItem * getFunction( const char * name );

  template<class R>
  void addFun0( R( *pf )(), const char * name ) {
    funcItem.insert( std::make_pair( name,
                                     new Function<OnExec0, R( * )() >( pf ) ) );
  }
  template<class R, class A1>
  void addFun1( R( *pf )( A1 ), const char * name ) {
    funcItem.insert( std::make_pair( name, new Function <
                                     OnExec1<A1> , R( * )( A1 ) > ( pf ) ) );
  }
  template<class R, class A1, class A2>
  void addFun2( R( *pf )( A1, A2 ), const char * name ) {
    funcItem.insert( std::make_pair( name, new Function <
                                     OnExec2<A1, A2> , R( * )( A1, A2 ) > ( pf )
                                   )
                   );
  }
  template<class R, class A1, class A2,class A3>
  void addFun3( R( *pf )( A1,A2,A3 ), const char * name ) {
    funcItem.insert( std::make_pair( name, new Function <
                                     OnExec3<A1,A2,A3>,
                                     R( * )( A1,A2,A3 )
                                    > ( pf )
                                   )
                   );
  }

  template<class R, class A1, class A2, class A3, class A4>
  void addFun4( R( *pf )( A1,A2,A3,A4), const char * name ) {
    funcItem.insert( std::make_pair(name, new Function <
                                    OnExec4<A1,A2,A3,A4>,
                                    R( * )( A1,A2,A3,A4) > ( pf )
                                   )
                   );
  }
  template<class R, class A1, class A2, class A3, class A4, class A5>
  void addFun5( R( *pf )( A1,A2,A3,A4,A5 ), const char * name ) {
    funcItem.insert( std::make_pair(name, new Function <
                                    OnExec5<A1,A2,A3,A4,A5>,
                                    R( * )( A1,A2,A3,A4,A5) > ( pf )
                                   )
                   );
  }

  template < class T>
  void addVar( T * t, const char * name ) {
    varItem.insert( std::make_pair( name, new Variable<T>( t ) ) );
  }

  template < class T>
  void addObject( T * t, const char * name ) {
    size_t len = strlen( name ) + 25;
    char * mask = new char[ 2* len ];
    char * tmp = mask + len;
    memset( mask, 0, 2 * len );

    sprintf( mask, "%s[%%i]", name );
    writer::puts( mask );
    //puts( mask );
    int i = 0;
    ItemIterator it;
    do {
      sprintf( tmp, mask, i );
      it = objectItem.find( tmp );

    }
    while( it != objectItem.end() );
    writer::printf( "inserting: '%s'\n", tmp );
    objectItem.insert( std::make_pair( tmp, new ParserObject<T>( t ) ) );

    delete mask;

  }

  template < class T, class R>
  void addMthd0( R( T::*pm )(), const char * name )
  {
    for( ItemIterator it = objectItem.begin(); it != objectItem.end(); ++it ) {
      ParserObject<T> * po = static_cast<ParserObject<T> *>( it->second );
      if( strcmp( typeid( T ).name(), po->getTypeName() ) == 0 )
      {
        po->addMthd0( pm, name );
        return;
      }
    }

  }
  template < class A1, class T, class R >
  void addMthd1( R( T::*pm )( A1 ), const char * name )
  {
    //ItemIterator it = objectItem.find( name );
    for( ItemIterator it = objectItem.begin(); it != objectItem.end(); ++it ) {
      ParserObject<T> * po = static_cast<ParserObject<T> *>( it->second );
      if( strcmp( typeid( T ).name(), po->getTypeName() ) == 0 )
      {
        po->addMthd1( pm, name );
        return;
      }
    }
  }

  char * getBuffer( int index, int len ) const
  {
    static char b[12][50];
    assert( len < 50 );
    memset( b[index], 0, 50 );
    return b[index];
  }


  friend void setParserVar( const char *, const char * );
protected:
private:
  typedef std::map<std::string, BaseParserItem *> ItemMapType;
  typedef ItemMapType::iterator ItemIterator;
  typedef ItemMapType::const_iterator ItemConstIterator;
  ItemMapType funcItem;
  ItemMapType varItem;
  ItemMapType objectItem;
};

} //namespace parserNs
using parserNs::Parser;

extern Parser parser;

