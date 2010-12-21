#pragma once
#include "precompiled.h"
#include "entity.h"
#include "messages/comunicator.h"


template <class T>
struct ReturnZeroPolicy {
  template < class W>
  const T * notFound(Entity e,W * w) const {
    return 0;
  }
  template < class W>
  T * notFound(Entity e,W * w) {
    return 0;
  }
};

template <class T>
struct AddEmptyPolicy {
  template < class W>
  const T * notFound(Entity e,W * w) const {
    printf("Entity: %i not found in component",e.getId() );
    w->records.push_back(new T() );
    w->records.back()->entity = e;
    return w->records.back();
  }
  template < class W>
  T * notFound(Entity e,W * w) {
    w->records.push_back(new T() );
    w->records.back()->entity = e;
    return w->records.back();
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
template<class T,class NotFoundPolicy = ReturnZeroPolicy<T> >
class BaseComponent : NotFoundPolicy
{
  template<class W>
  friend T* NotFoundPolicy::notFound(Entity , W* );
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
  T * get( Entity e )
  {
    for( __typeof( records.begin() ) it = records.begin(); it != records.end(); ++it ) {
      if(( *it )->entity == e )
        return ( *it );
    }//koniec for(records)
    return notFound(e,this);
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
    return 0;
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

  int32_t saveText(const char * filename)
  {
    std::string str;
    for(int i = 0; i < (int)records.size(); ++i ){
      str+="begin entity\n";
      str += records[i]->getAsString();
      str += "\nend entity\n\n";
    }/*koniec for (i)*/
    //tworzy pusty plik
    FILE * file = fopen(filename,"w" );
    fputs(str.c_str(),file);
    fclose(file);

    return 0;
  }
  int32_t loadText(const char * filename)
  {
    records.clear();  //niby niepotrzebne ale lepiej miec tego swiadomosc

    char * buffer = 0;
    size_t fileSize = 0;
    if (loadFileToBuffer(filename,&buffer,&fileSize) < 0)
      return -1;

    std::vector<char* > line;
    char * pch = strtok (buffer,"\n");

    while (pch != NULL)
    {
      printf ("%s\n",pch);
      line.push_back( pch );
      if(! strcmp(pch,"end entity") ){
        T t( &line[1] ); //tworzenie obiektu z odczytanych lini
        //t.entity = Entity();
        add(t.entity,t);
        line.clear();
      }
      pch = strtok (NULL, "\n");
    }
    delete []buffer;
    return 0;
  }

  int32_t saveBinary(const char * file);
  int32_t loadBinary(const char * file);

protected:
  Game * game;
  std::vector<T *> records;

}; // koniec BaseComponent


