#pragma once
#include "precompiled.h"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif /*define BUFFER_OFFSET */

/*! \class Vbo
    Parametr T to typ wierzchołka. Bufor ustawia wskażniki w zależności od stałych
    zdefiniowanych w strukturze. Wszystkie zestawy texcoordów muszą być jeden po drugim.

    Dostępne wartości do zdefiniowania w strukturze wierzchołka:

-   COORDS_COUNT                     ile coordynatów ma wierzchołek np x,y,z - 3
-   COORDS_TYPE                          typ coordów np. GL_FLOAT
-   COORDS_OFFSET                        offset od początku struktury w bajtach

-   TEXCOORDS_SETS_COUNT                ile zestawów texcoodów znajduje sie w strukturze, narazie obsluguje tylko 4
-   TEX_OFFSET                          offset pierwszego zestawu textur, zestawy muszą być jeden po drugim

-   TEX_0_COUNT                      ile texcoordów na zestaw 0, np 2
-   TEX_0_TYPE                          typ zmiennej zestawu np. GL_FLOAT

-   TEX_0_COUNT                      ile texcoordów na zestaw 0, np 2
-   TEX_1_TYPE                          typ zmiennej zestawu np. GL_FLOAT

-   TEX_2_COUNT
-   TEX_2_TYPE

-   TEX_3_COUNT
-   TEX_3_TYPE

-   COLORS_COUNT                        ile składowych koloru jest w wierzchołku: 0, 3 lub 4
-   COLORS_TYPE                         typ koloru np. GL_FLOAT
-   COLORS_OFFSET                       offset koloru od początku wierzchołka

Reszta w budowie. bedzie potrzebne to sie dopisze.

*/

namespace       //anonimowy namespace
{

/*
Konwersja z GL_TYPE na noramlne typy
*/
template<uint32_t GL_TYPE>
struct GlTypeToC {
};
template<>
struct GlTypeToC<GL_FLOAT> {
  typedef float Type;
  enum {   Size = sizeof( Type ) };
};
template<>
struct GlTypeToC<GL_DOUBLE> {
  typedef double Type;
  enum {   Size = sizeof( Type ) };
};
template<>
struct GlTypeToC<GL_INT> {
  typedef int32_t Type;
  enum {   Size = sizeof( Type ) };
};


template< uint32_t COORDS_COUNT, class T>
struct VertexPointer {
  static void setAndEnable() {
    glEnableClientState( GL_VERTEX_ARRAY );
    //              ile koordów na wierzchołek      GL_FLOAT,       stride,     offset
    glVertexPointer( COORDS_COUNT,             T::COORDS_TYPE,  sizeof( T ),  BUFFER_OFFSET( T::COORDS_OFFSET ) );
  }
  static void disable() {
    glDisableClientState( GL_VERTEX_ARRAY );
  }
};
template< class T >
struct VertexPointer<0, T> {
  static void setAndEnable() {
    glDisableClientState( GL_VERTEX_ARRAY );
  }
  static void disable() {}
};


template<uint32_t TEXCOORDS_SETS_COUNT, class T>
struct TexCoordPointer {
  static void enable() {}
  static void setAllSets() {}
  static void disable() {}
};

//glEnableClientState(GL_VERTEX_ARRAY);
//glVertexPointer(3, GL_FLOAT, 0, Vertices);
//glEnableClientState(GL_NORMAL_ARRAY);
//glNormalPointer(GL_FLOAT, 0, Normals);
//glClientActiveTexture(GL_TEXTURE1);
//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//glTexCoordPointer(2, GL_FLOAT, 0, TexCoords1);
//glClientActiveTexture(GL_TEXTURE2);
//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//glTexCoordPointer(2, GL_FLOAT, 0, TexCoords2);
//
//glDrawElements();
//
//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//glClientActiveTexture(GL_TEXTURE1);
//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//glDisableClientState(GL_NORMAL_ARRAY);
//glDisableClientState(GL_VERTEX_ARRAY);


template<class T>
struct TexCoordPointer<1, T> {
  static inline void setAllSets() {
    glClientActiveTexture( GL_TEXTURE0 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( T::TEX_0_COUNT, T::TEX_0_TYPE, sizeof( T ), BUFFER_OFFSET( T::TEX_OFFSET ) );
  }

  static inline void disable() {
    glClientActiveTexture( GL_TEXTURE0 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  }
};
template<class T>
struct TexCoordPointer<2, T> {

  static inline void setAllSets() {
    TexCoordPointer<1, T>::setAllSets();

    glClientActiveTexture( GL_TEXTURE1 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( T::TEX_1_COUNT, T::TEX_1_TYPE, sizeof( T ),
                       BUFFER_OFFSET( T::TEX_OFFSET
                                      + T::TEX_0_COUNT * GlTypeToC<T::TEX_0_TYPE>::Size
                                    )
                     );
  }

  static inline void disable() {
    glClientActiveTexture( GL_TEXTURE1 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    TexCoordPointer<1, T>::disable();
  }
};

template<class T>
struct TexCoordPointer<3, T> {

  static inline void setAllSets() {
    TexCoordPointer<2, T>::setAllSets();

    glClientActiveTexture( GL_TEXTURE2 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glTexCoordPointer( T::TEX_2_COUNT, T::TEX_2_TYPE, sizeof( T ),
                       BUFFER_OFFSET( T::TEX_OFFSET
                                      + T::TEX_0_COUNT * sizeof( typename GlTypeToC<T::TEX_0_TYPE>::Type )
                                      + T::TEX_1_COUNT * sizeof( typename GlTypeToC<T::TEX_1_TYPE>::Type )
                                    )
                     );
  }

  static inline void disable() {
    glClientActiveTexture( GL_TEXTURE2 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    TexCoordPointer<2, T>::disable();
  }
};
template<class T>
struct TexCoordPointer<4, T> {

  static inline void setAllSets() {
    TexCoordPointer<3, T>::setAllSets();

    glClientActiveTexture( GL_TEXTURE3 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glTexCoordPointer( T::TEX_3_TYPE, T::TEX_3_TYPE, sizeof( T ),
                       BUFFER_OFFSET( T::TEX_OFFSET
                                      + T::TEX_0_COUNT * sizeof( typename GlTypeToC<T::TEX_0_TYPE>::Type )
                                      + T::TEX_1_COUNT * sizeof( typename GlTypeToC<T::TEX_1_TYPE>::Type )
                                      + T::TEX_2_COUNT * sizeof( typename GlTypeToC<T::TEX_2_TYPE>::Type )
                                    )
                     );
  }

  static inline void disable() {
    glClientActiveTexture( GL_TEXTURE3 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    TexCoordPointer<3, T>::disable();
  }
};

template< uint32_t COLORS_COUNT, class T>
struct ColorPointer {
  static inline void enable() {
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( T::COLORS_COUNT, T::COLORS_TYPE, sizeof( T ), BUFFER_OFFSET( T::COLORS_OFFSET ) );
  }
  static inline void disable() {
    glDisableClientState( GL_COLOR_ARRAY );
  }

};
template<class T>
struct ColorPointer<0, T> {
  static inline void enable() {
    glDisableClientState( GL_COLOR_ARRAY );
  }
  static inline void disable() {

  }

};

/*! wersja ogólna dla GL_BUFFER_ARRAY_BUFFER
*/
template< uint32_t BUFFER_TYPE, class T>
struct BufferTypeSpecyfic {
  static inline void prepareDraw() {
    VertexPointer<T::COORDS_COUNT, T>::setAndEnable();
    TexCoordPointer<T::TEXCOORDS_SETS_COUNT, T>::setAllSets();
    ColorPointer<T::COLORS_COUNT, T>::enable();
  }
  static inline void afterDraw() {
    TexCoordPointer<T::TEXCOORDS_SETS_COUNT, T>::disable();
    VertexPointer<T::COORDS_COUNT, T>::disable();
    ColorPointer<T::COLORS_COUNT, T>::disable();
  }
};

/*! Wersja cpecjalizowana dla bufora indexów
*/
template< class T >
struct BufferTypeSpecyfic<GL_ELEMENT_ARRAY_BUFFER, T> {
  static inline void prepareDraw() {
    //glEnableClientState( GL_INDEX_ARRAY );
    //uwazac na to zero na koncu
    //glIndexPointer( T::INDEX_TYPE,0,0);
  }
  static inline void afterDraw() {
    //glDisableClientState( GL_INDEX_ARRAY );
  }
};

}//koniec anonimowego namespacka


/*!

    BUFFER_TYPE określa czy bufor to wierzchołki czy indexy,
    tak naprawde mogło by to być okreslane w czsie wykonania,
    mi i tak to ryba, ale nie trzymam zazwyczaj w jednym buforze buforów wierzchołków i indexów

    Przed wykonaniem jakiejkolwiek operacji na buforze wywołac bind,
    po zakonczeniu unbind
    /tparam T struktura wierzchołka ze zdefiniowanymi informacjami o wierzchołku i strukturze
    /tparam BUFFER_TYPE GL_ARRAY_BUFFER lub GL_ELEMENT_ARRAY_BUFFER
*/
template<typename T, uint32_t BUFFER_TYPE >
class Vbo
{
public:
  Vbo() : id( 0 ), usage( GL_STATIC_DRAW ), pMapped( 0 )
    , insertOffset( 0 ) {
    glGenBuffers( 1, &id );
  }
  Vbo( GLuint id_, GLenum usage_ ) : id( id_ ), usage( usage_ ), pMapped( 0 )
    , insertOffset( 0 ) {
  }
  ~Vbo() {
    glDeleteBuffers( 1, &id );
  }
  operator GLuint() {
    return id;
  }

  void reserve( uint32_t count_, GLenum usage_, const T * pData = 0 ) {
    count = count_;
    usage = usage_;

    glBufferData( BUFFER_TYPE, count * sizeof( T ), pData , usage );
  }
  /*!
      \return offset juz po dodaniu wierzchołków, nie mylić jednak z wskaźnikiem zapisu
              lub w przypadku błędu (np. za duzy offset) ERROR_CODE
  */
  uint32_t fill( uint32_t offsetInVerts, uint32_t vertsCount, const T * pData ) {
    if( offsetInVerts >= count ) {     //bufor mniejszy niz offset więc nie ma co wrzucać
      PRINT_ERROR("bufor mniejszy niz offset");
      return ERROR_CODE ;
    }
    offsetInVerts = std::min( offsetInVerts, count );
    vertsCount = std::min( vertsCount, count );
    glBufferSubData( BUFFER_TYPE, offsetInVerts * sizeof( T ), vertsCount * sizeof( T ), pData );
    return offsetInVerts + vertsCount;
  }
  /*! Dodaje dane zazaz po poprzednich, jesli bufor sie zapełni dane nie zostaną dodane,
      nalezy wtedy narysowac to co jest i wywołac setInserter(0) co przestawi
      wskaźnik zapisu do bufora na początek
      \return aktualny offset juz po dodaniu wierzchołków
  */
  uint32_t add( uint32_t vertsCount, const T * pData ) {
    if( fill( insertOffset, vertsCount, pData ) == -1 ) {
      return ERROR_CODE;
    }
    insertOffset += vertsCount;
    return insertOffset;
  }

  //! ustawia wskażnik zapisu do bufora, uzywany jest on w Vbo::add
  /*! nie wymaga bindowania
      \sa Vbo::add
      \sa Vbo::bind
  */
  void setInserter( uint32_t insertOffsetInVerts ) {
    insertOffset = std::min( insertOffsetInVerts, count - 1 );
  }

  /*!
      glMapBuffer czeka az karta wnarysuje to co ma w buforze, chcąc uniknąć
      czekania należy wcześniej wywwołać discard
      \param GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE.
  */
  T * map( GLenum access ) {
    if( pMapped ) {
      puts( "Najpierw unmapuj poprzedni wskaznik" );
      return 0;
    }
    pMapped = ( T* )glMapBuffer( BUFFER_TYPE, access );
    return pMapped;
  }
  bool unmap() {
    T * pMapTmp = pMapped;
    pMapped = 0;
    if( pMapTmp )
      return glUnmapBuffer( BUFFER_TYPE );
    return GL_FALSE;
  }
  T * getMappedPointer() {
    return pMapped;
  }
  void bind() {
    glBindBuffer( BUFFER_TYPE, id );
  }
  void unbind() {
    glBindBuffer( BUFFER_TYPE, 0 );
  }
  void discard() {
    glBufferData( BUFFER_TYPE, count * sizeof( T ), NULL, usage );
    setInserter(0);
  }

  void prepareDraw() {
    BufferTypeSpecyfic<BUFFER_TYPE, T>::prepareDraw();
  }

  void afterDraw() {
    BufferTypeSpecyfic<BUFFER_TYPE, T>::afterDraw();
  }
  //! podaje ile wierzchołków może sie zmiećcić w buforze
  uint32_t capacity() {
    return count;
  }

protected:
  GLuint id;
  GLenum usage;
  uint32_t count;
  T * pMapped;
  uint32_t insertOffset;
}; // koniec Vbo

#ifdef ITS_JUST_VERTEX_EXAMPLE
struct SpriteVertExample {
  float x, y;
  float u0, v0;
  float u1, v1;
  float padding[2];

  enum {
    COORDS_COUNT = 2,            //!< ile coordynatów ma wierzchołek
    COORD_TYPE      = GL_FLOAT,     //!< typ coordów
    COORDS_OFFSET    = 0,            //!< offset od początku struktury w bajtach

    TEXCOORDS_SETS_COUNT    = 2,    //!< ile zestawów texcoodów, zarazie obsluguje tylko 4
    TEX_OFFSET              = COORDS_COUNT * sizeof( float ),  //!< offset zestawu

    TEX_0_COUNT          = 2,    //!< ile texcoordów na zestaw 0
    TEX_0_TYPE              = GL_FLOAT,             //!< typ zmiennej zestawu

    TEX_1_COUNT          = 2,    //!< ile texcoordów na zestaw 1
    TEX_1_TYPE              = GL_FLOAT,             //!< typ zmiennej zestawu

  };
};
#endif /*ITS_JUST_VERTEX_EXAMPLE*/

