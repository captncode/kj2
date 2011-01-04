#pragma once
#include "precompiled.h"
#include "entity.h"
#include "messages/comunicator.h"

namespace NullWriterPolicy
{
inline void printf( const char *, ... ) {}
inline void puts( const char * ) {}
}

namespace writer = NullWriterPolicy;
//namespace writer = std;

template <class T>
struct ReturnZeroPolicy {
  template < class W>
  const T * notFound( Entity e, W * w ) const {
    return 0;
  }
  template < class W>
  T * notFound( Entity e, W * w ) {
    return 0;
  }
};

template <class T>
struct AddEmptyPolicy {
  template < class W>
  const T * notFound( Entity e, W * w ) const {
    writer::printf( "Entity: %i not found in component", e.getId() );
    w->records.push_back( new T() );
    w->records.back()->entity = e;
    return w->records.back();
  }
  template < class W>
  T * notFound( Entity e, W * w ) {
    w->records.push_back( new T() );
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

template<class T>
bool componentSortByEntity( T * l, T * r )
{
  return l->entity < r->entity;
}

//! \class BaseComponent
/*! */
template<class T, class NotFoundPolicy = ReturnZeroPolicy<T> >
class BaseComponent : NotFoundPolicy
{
  friend class Game;

  template<class W>
  friend T * NotFoundPolicy::notFound( Entity , W * );
public:
  BaseComponent( Game * game_ ) : game( game_ ) {}
  ~BaseComponent() {
    for( int i = 0; i < ( int )records.size(); ++i ) {
      delete records[i];
    }//koniec for (i)
  }
  template<class W>
  T* add( Entity e, const W & def ) {
    assert(e.getId() );
    records.push_back( new T( def, e, game ) );
    records.back()->entity = e;
    return records.back();
  }
  /*! ususwa jeden rekord, jeśli znaleziono zwraca true jest sens szukania
      dalej rekordów pod tym entity;
      jesli zwróciło fałsz to nieznaleziono i nic nie usunięto
  */
  bool remove( Entity e ,T * out = 0) {
    assert( e.getId() );
    for(int i = records.size()-1 ; i >= 0; --i ){
      if( records[i]->entity == e ) {
        if(out){
          *out = *records[i]; //kopiuje obiekt
        }//if out
        delete records[i];  //zwalnia wskaźnik
        records.erase( records.begin() + i );
        return true;
      } //if records[i]...
    }//koniec for (i)

    return false;
  }//void remove

  void clear() {
    for( int i = 0; i < ( int )records.size(); ++i ) {
      delete records[i];
    }/*koniec for (i)*/
    records.clear();
  }

protected:
  /*! przeszukiwanie od tyłu pozwala na trzymanie paru rekordów pod jednym
      entity
  */
  T * get( Entity e ) const
  {
    assert(e.getId() );
    for(int i = records.size() -1 ; i >= 0; --i ){
      if(records[i]->entity == e){
        return records[i];
      }
    }/*koniec for (i)*/

    return 0;
  }

  T * getOrAdd( Entity e )
  {
    assert(e.getId() );
    for(int i = records.size() -1 ; i >= 0; --i ){
      if(records[i]->entity == e){
        return records[i];
      }
    }/*koniec for (i)*/

    T tDef;
    add(e,tDef);
    return records.back();
  }

  T * getSure( Entity e ) const
  {
    //assert(e.getId() );
    for(int i = records.size() -1 ; i >= 0; --i ){
      if(records[i]->entity == e){
        return records[i];
      }
    }/*koniec for (i)*/
    //nie znaleziono, no to jazda :P
    static T nullObiect;
    nullObiect = T();   //konstruktor kopiujący - przywracanie domyślnych
                        //wartości
    return &nullObiect;
  }

  /*!niebezpieczne!!!
    jesli nie przeszukało się do końca (poprzestało na pierwszym znalezionym)
    to przy następnym wywołaniu z takim samym parametrem szukanie bedzie
    rozpoczęte od miejsca w którym zoastało poprzenio zakończone
    Pozwala jednak przechowywać i pobierać kilka rekordów dla jednego entity.
    Najlepiej jest więc używać tylko w pętli while
    `while( t = getNext(e) ){...}`
  */
  T * getNext( Entity e ) const
  {
    static Entity lastEntity = e;
    static int lastI = records.size() -1;

    int i;
    if( e == lastEntity )
      i = lastI;
    else
      i = records.size() -1;

    lastEntity = e;

    for( ; i >= 0; --i ) {
      if( records[i]->entity == e ) {
        lastI = i - 1;
        return records[i];
      }
    }/*koniec for (i)*/

    lastI = records.size() -1;
    return 0;
  }

  template<class W>
  T * overwrite( Entity e, const W & def ) {
    T * t = get( e );
    if( !t ) {
      return add( e, def );
    }
    else {
      t->~T();
      new( t )T( def, e, game );
      t->entity = e;
      return t;
    }
  }

  int32_t saveText( const char * filename ) const
  {
    std::string str;
    //std::sort(records.begin(),records.end(),componentSortByEntity<T> );
    char tmp[30];
    for( int i = 0; i < ( int )records.size(); ++i ) {
      if(records[i]->entity.saveIt == false){
        writer::printf("records[%i]->entity.saveIt == false\n",i);
        continue;
      }

      memset( tmp, 0, 30 );
      sprintf( tmp, "%i", i );
      str += "begin entity ";
      str += tmp;
      str += '\n';
      str += records[i]->getAsString();
      str += "\nend entity\n\n";
    }/*koniec for (i)*/

    //tworzy pusty plik
    FILE * file = fopen( filename, "w" );
    fputs( str.c_str(), file );
    fclose( file );

    return 0;
  }

  int32_t loadText( const char * filename, std::vector<Entity>& unit )
  {
    //clear();  //niby niepotrzebne ale lepiej miec tego swiadomosc

    char * buffer = 0;
    size_t fileSize = 0;
    if( loadFileToBuffer( filename, &buffer, &fileSize ) < 0 )
      return -1;

    std::vector<char * > line;
    char * pch = strtok( buffer, "\n" );
    bool recordBegan = false;

    while( pch != NULL )
    {
      writer::printf( "%s\n", pch );
      char beginText [] = "begin entity";
      if( strncmp(beginText,pch,std::min(sizeof(beginText)-1,strlen(pch))) == 0 ){
        recordBegan = true;
      }
      if (recordBegan){
        line.push_back( pch );
        if( ! strcmp( pch, "end entity" ) ) {
          recordBegan = false;
          T t( &line[1] ); //tworzenie obiektu z odczytanych lini
          /*gdy uzywam add to sie wpisy magicznie mnożą*/
          //overwrite( t.entity, t );
          add( t.entity, t);
          unit.push_back( t.entity );
          line.clear();
        }
      }
      pch = strtok( NULL, "\n" );
    }
    delete []buffer;
    return 0;
  }
  void afterLoad() const
  {
    for(int i = 0; i < (int)records.size(); ++i ){
      records[i]->afterLoad(game);
    }/*koniec for (i)*/
  }

//  void remapEntity( Entity dest, Entity source )
//  {
//    T * t = 0;
//    while( t = getNext( dest ) )
//    {
//      t->entity = source;
//    }
//  }

  int32_t saveBinary( const char * file );
  int32_t loadBinary( const char * file );

protected:
  Game * game;
  std::vector<T *> records;

}; // koniec BaseComponent


#define GET_AS_STRING_BODY_BEGIN() \
  char tmp[250]; \
  std::string out;
#define GET_AS_STRING_BODY_ADD(x,f) \
  memset(tmp,0,sizeof(tmp) ); \
  sprintf(tmp,f #x "\n ",x);     \
  out += tmp;
#define GET_AS_STRING_BODY_ADD_STRING(x)\
  out += x;       \
  out += " " #x       \
  out += "\n ";
#define GET_AS_STRING_BODY_END()\
  return out;


/*
moze kiedyś usprawnie tym serializacje tekstową
*/
typedef std::pair<char[20],uint32_t> SerializationTablePair;
#define SERIALIZATION_TABLE_BEGIN() \
  static SerializationTablePair* getSerializationTable(){   \
    static SerializationTablePair out[] = {

#define SERIALIZATION_TABLE_ADD(f,x,T)\
      SerializationTablePair(f,offsetof(T,x) ),

#define SERIALIZATION_TABLE_END()\
    };\
  }
