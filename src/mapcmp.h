#ifndef MAP_H
#define MAP_H

#include "component.h"
#include "vbo.h"


struct Vec2Quad;
struct AtlasInfo;

typedef Vec2 RenderVec2;
typedef Vec3 RenderVec3;

struct MapVertexBasic {
  RenderVec3 pos;
  RenderVec2 t0, t1;
  //RenderVec3 padding;
  float padding;
};

struct MapVertex : MapVertexBasic {
  enum {
    COORDS_COUNT = 3,                 //!< ile coordynatów ma wierzchołek
    COORDS_TYPE      = GL_FLOAT,       //!< typ coordów
    COORDS_OFFSET    = offsetof( MapVertexBasic, pos ), //!< offset od początku struktury w bajtach

    TEXCOORDS_SETS_COUNT    = 2,      //!< ile zestawów texcoodów, narazie obsluguje tylko 4
    TEX_OFFSET              = offsetof( MapVertexBasic, t0 ),

    TEX_0_COUNT             = 2,        //!< ile texcoordów na zestaw 0
    TEX_0_TYPE              = GL_FLOAT, //!< typ zmiennej zestawu

    TEX_1_COUNT             = 2,        //!< ile texcoordów na zestaw 1
    TEX_1_TYPE              = GL_FLOAT, //!< typ zmiennej zestawu

    COLORS_COUNT            = 0,
  };

};
class Game;
typedef Vec2 RenderVec2;

struct MapInfo {
  MapInfo() : mapName( "map/level1.map" ) {}
  MapInfo( const MapInfo & md, Entity entity_, Game * game );
  MapInfo( char * line[] );
  ~MapInfo();
  std::string getAsString();
  void afterLoad(Game * game);
private: void fillQuad(MapVertex mv[4],float x,float y,
                       const Vec2Quad& tex0, const Vec2Quad& tex1 );
public:
  int32_t getTileNumber(const Vec2& worldPos );
  //! zmienia tylko numer tekstury z atlasu
  int32_t setTileTexture( uint8_t texCoordSet,uint32_t atlasTextureNumber,
                                   const Vec2 & worldPos );


  void saveMapText();
  void saveMapText(const char* filename);

  Entity entity;
  int32_t startX_px, startY_px, startZ_px;
  float fStartZ;
  int32_t width, height;  //w kaflach
  std::string mapName, pngName, taiName;
  Vbo<MapVertex, GL_ARRAY_BUFFER> vbo;
  GLuint tex;
  const AtlasInfo * atlasInfo;
  std::vector<std::pair<uint32_t,uint32_t> > tile;
};


void generateMesh( Game * game );

class MapCmp : public BaseComponent<MapInfo>
{
public:
  static const uint32_t SCALE_X = 32;
  static const uint32_t SCALE_Y = 32;
  typedef BaseComponent<MapInfo> BaseType;

  MapCmp( Game * game );
  virtual ~MapCmp();

  using BaseType::get;
  using BaseType::loadText;

  void draw( const RenderVec2 & rv2 );

  MapInfo * getSector( const Vec2& worldPos );
  void saveAllText();
  void saveAllText(const char * baseName,const char* ext);

  void drawOnlyPaneNr(int32_t pane);

  static const uint32_t MAX_WIDTH  = 512;
  static const uint32_t MAX_HEIGHT = 512;

protected:
  Vbo<uint16_t, GL_ELEMENT_ARRAY_BUFFER> indexVbo;

  GLuint vShader,fShader,program;

  int32_t paneToDraw;//pole bitowe mowiace o tym którą warstwe rysować
};

template<>
inline
void getRelated<MapInfo>(const Entity e,MapInfo** out ){
  *out = g_game->getMapCmp()->get(e);
}
#endif // MAP_H
