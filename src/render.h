#pragma once
#include "precompiled.h"
#include "component.h"
#include "vec2rect.h"

#include "external/CBFG_BitmapFontClass.h"
#include "tai_loader.h"

#define FONTS_PATCH "fonts/"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif /*define BUFFER_OFFSET */


class Game;

template<typename T, uint32_t BUFFER_TYPE >
class Vbo;

typedef Vec2 RenderVec2;
typedef Vec3 RenderVec3;

class AtlasInfo
{
public:
  AtlasInfo() : size()
  {}

  /**
      \param  index odpowiada kolejnosci z pliku .tai zaczynając od 0
              i tak pierwsze coordy to 0, drugie 1 itd...
  */
  Vec2Quad getTileUV( uint32_t index )  const {
    const TaiLineData & tld = textureInfo[index];

    return Vec2Quad( RenderVec2( tld.woffset,       tld.hoffset ),               //leftUp
                     RenderVec2( tld.woffset + tld.width,  tld.hoffset ) , //rightUp
                     RenderVec2( tld.woffset,              tld.hoffset + tld.height ),  //leftDown
                     RenderVec2( tld.woffset + tld.width,  tld.hoffset + tld.height )
                   );
  }
  Vec2Quad getTileUV( const char * textureName )  const;
  uint32_t getTextureNumber( const char * textureName ) const;
  XY<uint32_t> getTextureSizePx( uint32_t index ) const ;

  GLuint tex;
  std::string filename;
  std::string infoFilename;
  //! rozmiar w pixelach
  XY<uint32_t>    size;
  //! jesli uzywam atlasu z nieregilarnym rozłorzeniem kafli tu trzymane są uv'ki
  std::vector< TaiLineData > textureInfo;

};//koniec struct AtlasInfo

//bool taiNaturalStringComp( const TaiLineData& l, const TaiLineData& r);

struct SpriteVertBase {
  float x, y, z;
  float tx, ty;
  uint8_t r, g, b, a;
  uint8_t padding[8];
};
//! dziedzicze tylko po ty moc użyć offsetof
struct SpriteVert : SpriteVertBase {
  enum {
    COORDS_COUNT        = 3,            //!< ile coordynatów ma wierzchołek
    COORDS_TYPE         = GL_FLOAT,     //!< typ coordów
    COORDS_OFFSET       = offsetof( SpriteVertBase, x ),         //!< offset od początku struktury w bajtach

    TEXCOORDS_SETS_COUNT    = 1,    //!< ile zestawów texcoodów, zarazie obsluguje tylko 4
    TEX_OFFSET              = offsetof( SpriteVertBase, tx ),   //!< offset pierwszego zestawu textur

    TEX_0_COUNT             = 2,            //!< ile texcoordów na zestaw 0
    TEX_0_TYPE              = GL_FLOAT,     //!< typ zmiennej zestawu

    COLORS_COUNT            = 4,            //!< zero w tym miejscu powoduje ze color ni bedzie brany pod uwage
    //! ale struktura powinna byc wyrównana do 32
    COLORS_TYPE             = GL_UNSIGNED_BYTE,
    COLORS_OFFSET           = offsetof( SpriteVertBase, r ),
  };
};

//! \struct Sprite
/*! */
struct SpriteRenderInfo {
  GLuint tex;
  SpriteVert  v[4];
}; // koniec struktury Sprite

inline
bool byTexture( const SpriteRenderInfo & l, const SpriteRenderInfo & r )
{
  return l.tex < r.tex;
}

inline
bool byTexturePtr( const SpriteRenderInfo * l, const SpriteRenderInfo * r )
{
  return l->tex < r->tex;
}

inline
bool byDepth( const SpriteRenderInfo & l, const SpriteRenderInfo & r )
{ //narazie wszystkie 4 wierzchołki mają taka samą głębokość
  return l.v[0].z < r.v[0].z;
}

/*!
    \param [in,out] inSurf powierzchnia która chce skonwertować na texture ogl
                    należy paniętać o zwolnieniu jej po konwersji, albo kiedy indziej.
    \param [out]    out_tex textura ogl
*/
int32_t surfaceToTexture( SDL_Surface * inSurf, GLuint * out_tex );




struct SpriteDef
{
  SpriteDef();

  Entity shape;   //prostokąt na którm bedzie rozciągnięta tekstura

  std::string atlasFile;    //nazwa atlasu
  std::string atlasInfoFile;  //nazwa pliku texture atlas info z rozszerzeniem
  std::string textureName;  //nazwa textury w atlasie
  uint32_t coordSpace;
  uint32_t color;
  bool visible;

};
struct SpriteInfo : public SpriteDef
{
  SpriteInfo() {}
  SpriteInfo( const SpriteDef & r, Entity e, Game * );
  SpriteInfo( char * line[] );
  std::string getAsString() const ;
  void afterLoad(Game * game);

  Entity entity;

  GLuint tex;
  uint32_t textureNumber;
};

struct ShapeDef;

//! \class SpriteCmp
/*! */
class SpriteCmp : public BaseComponent<SpriteInfo>
{
  typedef BaseComponent<SpriteInfo> BaseType;
public:
  SpriteCmp( Game * game_ );
  ~SpriteCmp() {}

  using BaseType::add;
//  bool get(Entity e)
//  {
//    return (bool)( BaseType::get(e) );
//  }
  void draw( Entity e , const ShapeDef& shapeDef );
  void forceDraw( Entity e , const ShapeDef& shapeDef);
  void drawAll();
  void setColor( Entity e, uint32_t color ) const ;
  void setVisibility ( Entity e, bool val) const ;
  const AtlasInfo * getAtlas(Entity e) const ;
  int32_t getTextureSizePx( Entity e ,XY<uint32_t>* out) const;

  void drawRect( const struct ShapeDef & d, uint32_t argb );

  using BaseType::get;  //jednak pozwalam, ale w kazdej chwili moge ukryc
  using BaseType::getOrAdd;
  using BaseType::getSure;
  using BaseType::overwrite;
  using BaseType::loadText;
  using BaseType::saveText;

//protected:
  SpriteInfo untexturedSprite;
}; // koniec SpriteCmp

inline
void getRelated(const Entity e,SpriteInfo** out );


struct TextInfo {
  TextInfo();
  TextInfo( const TextInfo & ti, Entity e, Game * ) : entity( e ), text( ti.text )
    , position( ti.position ) , color( ti.color ), font( ti.font )
    , coordSpace( ti.coordSpace ), depth( ti.depth ), visible( ti.visible )
  {}
  TextInfo( char * line[] );
  std::string getAsString();
  void afterLoad(Game * game);

  Entity entity;
  std::string text;
  RenderVec2 position;
  uint32_t color;
  uint32_t font;
  uint32_t coordSpace;

  int16_t depth;
  bool visible;
};


class TextCmp : public BaseComponent<TextInfo, AddEmptyPolicy<TextInfo> > {

  typedef BaseComponent<TextInfo, AddEmptyPolicy<TextInfo> > BaseType;
public:
  TextCmp( Game * game_ );
  ~TextCmp() {}

  //którą wersje wybrać?
  //ustawianie atrybutu poprzez component
  //czy pobieranie danych jednostki i "reczna" modyfikacja ?

  //void setPosition(Entity e, RenderVec2 pos);
  using BaseType::get;
  using BaseType::getOrAdd;
  using BaseType::getSure;
  using BaseType::add;
  using BaseType::loadText;
  using BaseType::saveText;

  int32_t loadFont( const char * file ) ;

  void drawAll();
  void draw( Entity e );

  int getTextWidth( const char *, uint32_t font, uint32_t len = uint32_t( -1 ) )const;
  int getFontHeight( uint32_t font ) const;

  const static uint32_t   consolasFont = 0;

protected:
  //załadowane fonty
  std::vector<CBitmapFont>    bitmapFont;
};

inline
void getRelated(const Entity e,TextInfo** out );



/*  Klasa ułatwiajaca rysowanie, zestaw metod do rysowania,
    dostarcza 'wysoko'poziomowe rysowanie
*/
class Render
{
public:
  //! określa przestrzeń (projekcji) koordów
  /*!*/
  enum CoordSpace_e {
    SCREEN_COORD,   //!< coordy są w przestrzeni ekranu
    WORLD_COORD     //!< coordy w przestrzeni świata
  };//koniec enum CoordSpace_e

  typedef std::map<GLuint, AtlasInfo>::iterator AtlasMap_it;
  const static Vec2Quad fullTexture;
  const static uint32_t   white;
  const static float  inv255;
  const static uint32_t   consolasFont = 0;
  const static AtlasInfo atlasNotFound ;

  Render( Game * game_, const XY<uint32_t>& wndDim_, uint32_t depth_,
          bool fullscreen_ );
  ~Render();

  /*!
      \param wndDim rozmiar okna w pixelach(normalnie ekranowych)
  */
  SDL_Surface * initOGL( const XY<uint32_t>& wndDim, uint32_t colorDepth, bool fullscreen );
  bool wasInit() const {return wasinit; }

  void shutdownOGL();

  /*! ustawia centrum(w/2,h/2) ekranu w podanym punkcie świata
      \sa screenOrig
      \param so punkt w kordach świata */
  void setScrCentre( const RenderVec2 & so );

  /*! ustawia lewy górny róg ekranu w podanym punkcie świata
      \sa screenOrig
      \param so punkt w kordach świata */
  void setScrOrig( const RenderVec2 & so );

  /*! zwraca współrzedną lewego górnego rogu ekranu, w jednostkach świata
   \sa screenOrig */
  const RenderVec2 & getScrOrig()    const   {return screenOrig; };

  /*! zwraca wspołrzedne środka ekranu w układzie współrzednych świata*/
  RenderVec2 getScrCentre()         const   {
    return RenderVec2( screenOrig.x + 0.5f * invZoom * ( float( winDim.x ) ),
                       screenOrig.y + 0.5f * invZoom * ( float( winDim.y ) )
                     );
  }

  /*! zwraca rozmiar ekranu w uwaga! współrzednych świata, czyli rozmiar okna przeskalowany o zoom*/
  RenderVec2 getWorldViewDim()     const   {
    return RenderVec2((( float )winDim.x ) * invZoom,
                      (( float )winDim.y ) * invZoom );
  };

  const XY<uint32_t>& getWindowDim()   const {
    return winDim;
  }

  const RenderVec2 getDeskDim()    const   {return RenderVec2( deskDim.x, deskDim.y ); };

  //! przekształa wsółrzedne ekranowe na współrzedne świata gry
  RenderVec2 scrToWorld( const RenderVec2 & v ) const   {return screenOrig + v; };

  RenderVec2 scrToWorld( float x, float y )const   {return RenderVec2( screenOrig.x + x, screenOrig.y + y ) ; };

  //! przybliza lub oddala widok
  void zoomRational( float step ) {
    zoom += step;
    if( zoom && step ) {
      invZoom = 1.f / zoom;
    }
    DEBUG_PRINTF( 1.f / invZoom, "%f (Render)\n" );
  }

  void zoomLinear( float step ) {
    zoom += step;
    if( zoom && step ) {
      invZoom += step;
    }
    DEBUG_PRINTF( 1.f / invZoom, "%f (Render)\n" );
  }

  float getInvZoom() const {
    return invZoom;
  }
  void setZoom( float z ) {
    if( z ) {
      zoom = z;
      invZoom = 1.f / zoom;
    }
  }

  uint32_t beginDraw( const RenderVec2 & centre );
  uint32_t endDraw();
  void sortAndDrawSprites();

  const AtlasInfo * loadAtlas( const char * name, const char * taiName );

  //zwalnia texture z pamieci
  void unload( GLuint * tex );

  int32_t drawSprite( GLuint texture, const Vec2Quad & uv, const Vec2Quad & pos,
                      const CoordSpace_e space,
                      uint32_t color = white, int16_t depth = 0 );

  int32_t drawAtlas( GLuint texture, uint32_t tileIndex, const Vec2Quad & pos,
                     const CoordSpace_e space,
                     uint32_t color = white, int16_t depth = 0 );

  const AtlasInfo * getAtlas( GLuint texID ) {
    for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
      if( it->tex == texID )
        return &( *it );
    }//koniec for(atlas)
    PRINT_ERROR( "nie znaleziono atlasu!\n" );
    return 0;
  }
  const AtlasInfo * getAtlas( const char * filename );
  const AtlasInfo * getAtlas( const char * filename, const char * infoFilename );

protected:
  Game * game;
  XY<uint32_t> winDim, deskDim;
  uint32_t colorDepth;
  float   zoom, invZoom;
  bool fullscreen;            //!< true - full; false - okno
  bool wasinit;               //!< czy juz zainicializowano
  char padding[2];

  /*  początek układu współrzednych ekranu(tego którego używa SDL)
      wyrażony we współrzednych świata gry (logicznych)*/
  RenderVec2 screenOrig;

  //! \var spriteIbo indexy odrazu wrzucam do bufora, i generowane one sa dla GL_QUADS
  /*! nawet jak wierzchołki przy wrzucaniu z wektora do vbo zmienią pozycje to indexy zostają nietknięte
      narazie to sobie daruje
  */
  Vbo<SpriteVert, GL_ARRAY_BUFFER>*             spriteVbo;
  Vbo<uint16_t, GL_ELEMENT_ARRAY_BUFFER>*       spriteIbo;

  std::vector< SpriteRenderInfo > spriteToDraw;

  std::vector<AtlasInfo> atlas;

  //załadowane fonty
  std::vector<CBitmapFont>    bitmapFont;
}; // koniec Render


void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);
GLint createAndCompileShader(const char* filename,GLuint shaderType);
