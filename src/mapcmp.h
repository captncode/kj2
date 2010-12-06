#ifndef MAP_H
#define MAP_H

#include "component.h"
#include "vbo.h"

typedef Vec2 RenderVec2;
typedef Vec3 RenderVec3;

struct MapVertexBasic{
  RenderVec3 pos;
  RenderVec2 t0,t1;
  RenderVec3 padding;
};

struct MapVertex : MapVertexBasic{
  enum {
    COORDS_COUNT = 3,                 //!< ile coordynatów ma wierzchołek
    COORDS_TYPE      = GL_FLOAT,       //!< typ coordów
    COORDS_OFFSET    = 0,             //!< offset od początku struktury w bajtach

    TEXCOORDS_SETS_COUNT    = 2,      //!< ile zestawów texcoodów, zarazie obsluguje tylko 4
    TEX_OFFSET              = offsetof(MapVertexBasic,t0),

    TEX_0_COUNT             = 2,        //!< ile texcoordów na zestaw 0
    TEX_0_TYPE              = GL_FLOAT, //!< typ zmiennej zestawu

    TEX_1_COUNT             = 2,        //!< ile texcoordów na zestaw 1
    TEX_1_TYPE              = GL_FLOAT, //!< typ zmiennej zestawu

    COLORS_COUNT            = 0,
  };

};
class Game;
typedef Vec2 RenderVec2;

struct MapDef {
	std::string filename;
};

struct MapInfo {
	MapInfo( const MapDef & md, Entity entity_, Game * game );
	Entity entity;

	int startX,startY,startZ,width,height;
	std::string filename, taiFilename;
	Vbo<MapVertex,GL_ARRAY_BUFFER> vbo;
	const struct AtlasInfo * atlasInfo;
};

void generateMesh( Game * game );

class MapCmp : public BaseComponent<MapInfo>
{
public:
	MapCmp( Game * game );
	virtual ~MapCmp();

	void draw( const RenderVec2 & rv2 );

	static const uint32_t MAX_WIDTH  = 512;
  static const uint32_t MAX_HEIGHT = 512;

protected:
  Vbo<uint16_t,GL_ELEMENT_ARRAY_BUFFER> indexVbo;
};

#endif // MAP_H
