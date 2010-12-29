#include "mapcmp.h"
#include "main.h"
#include "render.h"

MapInfo::MapInfo( const MapInfo & md, Entity entity_, Game * game ) :
  entity( entity_ ), mapName( md.mapName ), startX( md.startX ), startY( md.startY )
  , startZ( md.startZ )
{
  char * buffer = 0;
  size_t bufferSize = 0;
  if( loadFileToBuffer( mapName.c_str(), &buffer, &bufferSize ) < 0 ) {
    PRINT_ERROR( "nie zaladowano mapy: " );
    puts( mapName.c_str() );
    return;
  }
  char * ptr = buffer;

  char tmp[250];
//  int intStartZ;
//  sscanf(ptr,"%d %d %d",&startX,&startY,&intStartZ);

  fStartZ = startZ / float(( 1 << 16 ) - 1 );
  ptr = strchr( ptr, '\n' ) + 1; //do nastepnej lini
  sscanf( ptr, "%d %d", &width, &height );

  ptr = strchr( ptr, '\n' ) + 1; //do nastepnej lini
  memset( tmp, 0, 250 );
  sscanf( ptr, "%250s", tmp );
  pngName = tmp;

  ptr = strchr( ptr, '\n' ) + 1;
  memset( tmp, 0, 250 );
  sscanf( ptr, "%250s", tmp );
  taiName = tmp;

  ptr = strchr( ptr, '\n' ) + 1;

  std::vector<int> value;
  value.resize( width * height );
  for( int i = 0; i < ( int )width * height; ++i ) {
    int scanned = 0;
    /*int read = */sscanf( ptr, "%d", &scanned );
    //odejmuje jeden bo linie w pliku numerowane są od 1 a w tablicy indeksuje
    //od zera
    value[i] = scanned ;

    ptr = strchr( ptr, ',' ) + 1;

  }/*koniec for (i)*/

  delete [] buffer;

  Render * r = game->getRender();
  const AtlasInfo * atlasInfo = r->getAtlas( pngName.c_str(), taiName.c_str() );
  if( !atlasInfo ) {
    atlasInfo = r->loadAtlas( pngName.c_str(), taiName.c_str() );
    if( !atlasInfo ) {
      PRINT_ERROR( "nie udalo sie zalodowac atlasu: " );
      fputs( pngName.c_str(), stdout );
      puts( taiName.c_str() );
    }
  }
  tex = atlasInfo->tex;

  MapVertex * vert = new MapVertex[width*height*4];
  int i = 0;
  const int32_t scaleX = 32;
  const int32_t scaleY = 32;
  for( int h = 0; h < ( int )height; ++h ) { //poziome paski
    for( int w = 0; w < ( int )width; ++w ) {
      /*w + h*width = i/4*/
      Vec2Quad v2q = atlasInfo->getTileUV( value[w + h*width] );

      vert[i +0].pos =
        RenderVec3( w ,  h,  0.f ) + RenderVec3( startX, startY, fStartZ );
      vert[i +0].pos.x *= scaleX;
      vert[i +0].pos.y *= scaleY;
      vert[i +0].t0 =  v2q.upLeft;

      vert[i +1].pos =
        RenderVec3( w + 1, h,  0.f ) + RenderVec3( startX, startY, fStartZ );
      vert[i +1].pos.x *= scaleX;
      vert[i +1].pos.y *= scaleY;
      vert[i +1].t0 =  v2q.upRight;

      vert[i +2].pos =
        RenderVec3( w + 1, h + 1, 0.f ) + RenderVec3( startX, startY, fStartZ );
      vert[i +2].pos.x *= scaleX;
      vert[i +2].pos.y *= scaleY;
      vert[i +2].t0 =  v2q.downRight;

      vert[i +3].pos =
        RenderVec3( w,  h + 1, 0.f ) + RenderVec3( startX, startY, fStartZ );
      vert[i +3].pos.x *= scaleX;
      vert[i +3].pos.y *= scaleY;
      vert[i +3].t0 =  v2q.downLeft;
      i += 4;
    }/*koniec for (w)*/
  }/*koniec for (h)*/

  vbo.bind();
  vbo.reserve( width * height * 4, GL_STATIC_DRAW, vert );
  vbo.unbind();

  delete []vert;
}
MapInfo::MapInfo( char * line[] )
{
  int e;
  sscanf( line[0], "%i", &e );
  entity = Entity( e );

  int l = 2;
  char * begin = 0;
  char * end = 0;
  std::string * str[] = {&mapName};

  for( int i = 0; i < sizeof( str ) / sizeof( str[0] ); ++i ) {
    begin = strchr( line[l-1], '"' ) + 1;
    end = strchr( begin, '"' );
    *( str[i] ) = begin;

    while( !end ) {
      *( str[i] ) += '\n';
      end = strchr( line[l], '"' );
      *( str[i] ) += line[l];
      ++l;
    }
    pop_back( *( str[i] ) ); //usuwam ostatni nawias
  }
  l -= 2;

  sscanf( line[l+2], "%d %d %d", &startX, &startY, &startZ );
  //startZ = intStartZ/float( (1<<16) -1);

}

MapInfo::~MapInfo()
{
  //vbo samo sie zwolni
  //texture zwolni render
}

std::string MapInfo::getAsString()
{
  char tmp[250] = {};

  sprintf( tmp, "%i MapInfo\n \"", entity.getId() );
  std::string out( tmp );
  out += mapName;
  out += "\"\n ";
  //startZ = intStartZ/float( (1<<16) -1);
  //int intStartZ = startZ / float( (1<<16) -1);

  memset( tmp, 0, 250 );
  sprintf( tmp, " %i %i %i startX startY startZ", startX, startY, startZ );
  out += tmp;

  return out;

}
void MapInfo::afterLoad(Game * game)
{

}


MapCmp::MapCmp( Game * game ) : BaseComponent<MapInfo>( game )
{
  uint16_t * ind = new uint16_t[MAX_WIDTH*MAX_HEIGHT*6];
  int i = 0;
  int k = 0;
  for( int h = 0; h < ( int )MAX_HEIGHT; h += 1 ) {
    for( int w = 0; w < ( int )MAX_WIDTH; ++w ) {
      ind[k +0] = i + 0;
      ind[k +1] = i + 1;
      ind[k +2] = i + 2;

      ind[k +3] = i + 0;
      ind[k +4] = i + 2;
      ind[k +5] = i + 3;
      i += 4;
      k += 6;
    }/*koniec for (w)*/
  }/*koniec for (h)*/
  indexVbo.bind();
  indexVbo.reserve( MAX_WIDTH * MAX_HEIGHT * 6, GL_STATIC_DRAW, ind );
  indexVbo.unbind();

  delete []ind;
}
MapCmp::~MapCmp()
{
  //dtor
}

void MapCmp::draw( const RenderVec2 & rv2 )
{
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  indexVbo.bind();
  indexVbo.prepareDraw();
  for( int i = 0; i < ( int )records.size(); ++i ) {
    MapInfo * mi = records[i];
    //AtlasInfo * ai = game->getRender()->getAtlas(mi->)
    glBindTexture( GL_TEXTURE_2D, mi->tex );

    mi->vbo.bind();
    mi->vbo.prepareDraw();
    glDrawRangeElements( GL_TRIANGLES, 0, indexVbo.capacity(),
                         mi->width * mi->height * 6, GL_UNSIGNED_SHORT, 0 );
    mi->vbo.afterDraw();
    mi->vbo.unbind();
  }/*koniec for (i)*/
  indexVbo.afterDraw();
  indexVbo.unbind();

  glDisable( GL_BLEND );
}
