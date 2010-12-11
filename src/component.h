#pragma once
#include "precompiled.h"
#include "entity.h"
#include "messages/comunicator.h"


template <class T>
struct ReturnZeroPolicy {
  const T * notFound() const {
    return 0;
  }
  T * notFound() {
    return 0;
  }
};

template <class T>
struct ThrowExceptionPolicy {       //dokonczyc, ale nieuzywać ;)
  const T * notFound() const {
    //return 0;
  }
  T * notFound() {
    //return 0;
  }
};

template <class T>
struct NullObiectPolicy {
  const T * notFound() const {
    return getNullObiect() ;
  }
  T * notFound() {
    return getNullObiect() ;
  }
  //dzieki temu nie musze deklarować nullObiektu w .cpp
  static inline T * getNullObiect() {
    //static T nullObiect;
    //return &nullObiect;
    static char place[sizeof( T )] = {};
    return reinterpret_cast<T *>( place );
  }

};

class Game;

//! \class BaseComponent
/*! */
template<class T>
class BaseComponent
{
public:
  BaseComponent( Game * game_ ) : game( game_ ) {}
  ~BaseComponent() {
    for( int i = 0; i < ( int )records.size(); ++i ) {
      delete records[i];
    }//koniec for (i)
  }
  template<class W>
  void add( Entity e, const W & def ) {
    records.push_back( new T( def, e, game ) );
    records.back()->entity = e;
  }
  void remove( Entity e ) {
    for( int i = 0; i < ( int )records.size(); ++i ) {
      if( records[i]->entity == e ) {
        delete records[i];
        records.erase( records.begin() + i );
        --i;
      }
    }//koniec for (i)
  }

protected:
  T * get( Entity e ) const
  {
    for( __typeof( records.begin() ) it = records.begin(); it != records.end(); ++it ) {
      if(( *it )->entity == e )
        return ( *it );
    }//koniec for(records)
    return 0;
  }
  T * getNext( Entity e ) const
  {
    static Entity lastEntity = e;
    static int lastI = 0;

    int i;
    if( e == lastEntity)
      i = lastI;
    else
      i = 0;

    lastEntity = e;

    for(; i < (int) records.size(); ++i ){
      if( records[i]->entity == e ){
        lastI = i + 1;
        return records[i];
      }

    }/*koniec for (i)*/
  }

  template<class W>
  bool overwrite( Entity e, const W & def ) {
    T * t = get( e );
    if( !t ) {
      add( e, def );
      return false;
    }
    else {
      t->~T();
      new( t )T( def );
      t->entity = e;
      return true;
    }
  }

protected:
  Game * game;
  std::vector<T *> records;

}; // koniec BaseComponent


