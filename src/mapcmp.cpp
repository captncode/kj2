#include "main.h"
#include "mapcmp.h"
#include "render.h"

MapInfo::MapInfo( const MapInfo & md, Entity entity_, Game * game ) :
  entity( entity_ ), mapName( md.mapName ), startX_px( md.startX_px )
  , startY_px( md.startY_px )
  , startZ_px( md.startZ_px )
{
  char * buffer = 0;
  size_t bufferSize = 0;
  if( loadFileToBuffer( mapName.c_str(), &buffer, &bufferSize ) < 0 ) {
    PRINT_ERROR( "nie zaladowano mapy: " );
    puts( mapName.c_str() );
    return;
  }
  if( bufferSize == 0 ) {
    startX_px = 0;
    startY_px = 0;
    startZ_px = 0;
    fStartZ = startZ_px / float(( 1 << 16 ) - 1 );
    width = 64;
    height = 64;
    pngName = "img/by_ftorek.png";
    taiName = "img/by_ftorek.tai";
    for( int i =0 ; i < width * height; ++ i )
    { //wykona sie jesli z pliku wczytało za mało
      tile.push_back( std::make_pair( 82, 15 ) );
    }
  }
  else{
    char * ptr = buffer;

    char tmp[250];
  //  int intStartZ;
  //  sscanf(ptr,"%d %d %d",&startX,&startY,&intStartZ);

    fStartZ = startZ_px / float(( 1 << 16 ) - 1 );
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

    //std::vector<std::pair<int,int> > tile;
    tile.reserve( width * height );

    int i = 0;
    for( ; i < ( int )width * height; ++i ) {
      int back = 0;
      int front = 0;
      int read = sscanf( ptr, "%d %d", &back, &front );
      if( read == EOF || read < 2 ) {
        //prawdopodobnie juz koniec mapy, trzeba reszte uzupwłnic czymś domyślnym
        break;
      }
      tile.push_back( std::make_pair( back, front ) );

      ptr = strchr( ptr, ',' ) + 1;
    }/*koniec for (i)*/
    for( ; i < width * height; ++ i )
    { //wykona sie jesli z pliku wczytało za mało
      tile.push_back( std::make_pair( 82, 15 ) );
    }
  }//else

  delete [] buffer;
  assert( width < MapCmp::MAX_WIDTH );
  assert( height < MapCmp::MAX_HEIGHT );

  Render * r = game->getRender();
  //const AtlasInfo * atlasInfo = r->getAtlas( pngName.c_str(), taiName.c_str() );
  atlasInfo = r->getAtlas( pngName.c_str(), taiName.c_str() );
  if( !atlasInfo ) {
    atlasInfo = r->loadAtlas( pngName.c_str(), taiName.c_str() );
    if( !atlasInfo ) {
      PRINT_ERROR( "nie udalo sie zalodowac atlasu: " );
      fputs( pngName.c_str(), stdout );
      puts( taiName.c_str() );
    }
  }
  tex = atlasInfo->tex;

  MapVertex * vert = new MapVertex[tile.size()*4];
  int v = 0, k = 0;
//  const int32_t MapCmp::SCALE_X = 32;
//  const int32_t MapCmp::SCALE_Y = 32;
  for( int h = 0; h < ( int )height; ++h ) { //poziome paski
    for( int w = 0; w < width && k < tile.size(); ++w, k += 1 ) {
      /*w + h*width = i/4*/
//      Vec2Quad v2q0 = atlasInfo->getTileUV( tile[w + h*width] );
//      Vec2Quad v2q1 = atlasInfo->getTileUV( tile[w + h*width + 1] );
      Vec2Quad v2q0 = atlasInfo->getTileUV( tile[k].first );
      Vec2Quad v2q1 = atlasInfo->getTileUV( tile[k].second );
      fillQuad( vert + v, w, h, v2q0, v2q1 );

      v += 4;
    }/*koniec for (w)*/
  }/*koniec for (h)*/

  vbo.bind();
  vbo.reserve( tile.size() * 4, GL_STATIC_DRAW, vert );
  vbo.unbind();

  delete []vert;
}
MapInfo::MapInfo( char * line[] )
{
  uint32_t e = 0;
  sscanf( line[0], "%u", &e );
  assert( e );
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

  sscanf( line[l+2], "%i %i %i", &startX_px, &startY_px, &startZ_px );
  startX_px *= ( int32_t )MapCmp::SCALE_X;
  startY_px *= ( int32_t )MapCmp::SCALE_Y;

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
  sprintf( tmp, " %i %i %i startX startY startZ",
           startX_px / ( int32_t )MapCmp::SCALE_X,
           startY_px / ( int32_t )MapCmp::SCALE_Y, startZ_px );
  out += tmp;

  return out;

}
void MapInfo::afterLoad( Game * game )
{

}

void MapInfo::fillQuad( MapVertex vert[4], float x, float y,
                        const Vec2Quad & tex0, const Vec2Quad & tex1 )
{
  vert[0].pos = RenderVec3( x ,  y,  0.f );
  vert[0].pos.x *= MapCmp::SCALE_X;
  vert[0].pos.y *= MapCmp::SCALE_Y;
  vert[0].pos += RenderVec3( startX_px, startY_px, fStartZ );
  vert[0].t0 =  tex0.upLeft;
  vert[0].t1 =  tex1.upLeft;

  vert[1].pos = RenderVec3( x + 1, y,  0.f );
  vert[1].pos.x *= MapCmp::SCALE_X;
  vert[1].pos.y *= MapCmp::SCALE_Y;
  vert[1].pos += RenderVec3( startX_px, startY_px, fStartZ );
  vert[1].t0 =  tex0.upRight;
  vert[1].t1 =  tex1.upRight;

  vert[2].pos = RenderVec3( x + 1, y + 1, 0.f );
  vert[2].pos.x *= MapCmp::SCALE_X;
  vert[2].pos.y *= MapCmp::SCALE_Y;
  vert[2].pos += RenderVec3( startX_px, startY_px, fStartZ );
  vert[2].t0 =  tex0.downRight;
  vert[2].t1 =  tex1.downRight;

  vert[3].pos = RenderVec3( x,  y + 1, 0.f );
  vert[3].pos.x *= MapCmp::SCALE_X;
  vert[3].pos.y *= MapCmp::SCALE_Y;
  vert[3].pos += RenderVec3( startX_px, startY_px, fStartZ );
  vert[3].t0 =  tex0.downLeft;
  vert[3].t1 =  tex1.downLeft;
}

XY<int32_t> worldCoordToTileCoords( const Vec2 & world ) {
  return XY<int32_t>( world.x / ( float )( MapCmp::SCALE_X ),
                      world.y / ( float )( MapCmp::SCALE_Y ) );
}

void checkPrintGlError()
{
  int err = glGetError();
  const char * errText = 0;
  switch( err ) {
    case GL_INVALID_ENUM:
      errText = "GL_INVALID_ENUM: given when an enumeration parameter \
contains an enum that is not allowed for that function.";
      break;
    case GL_INVALID_VALUE: errText = "GL_INVALID_VALUE: given when a numerical \
parameter does \
not conform to the range requirements that the function places upon it.";
      break;
    case GL_INVALID_OPERATION: errText = "GL_INVALID_OPERATION: given when \
the function in question \
cannot be executed because of state that has been set in the context.";
      break;
    case GL_STACK_OVERFLOW: errText = "GL_STACK_OVERFLOW:: given when a stack \
pushing operation \
causes a stack to overflow the limit of that stack's size.";
      break;
    case GL_STACK_UNDERFLOW: errText = "GL_STACK_UNDERFLOW:given when a stack \
popping operation\
is given when the stack is already at its lowest point.";
      break;
    case GL_OUT_OF_MEMORY: errText = "GL_OUT_OF_MEMORY: given when performing \
an operation that \
can allocate memory, when the memory in question cannot be allocated.";
      break;
    case GL_TABLE_TOO_LARGE: errText = "GL_TABLE_TOO_LARGE :( if the optional \
imaging subset( GL_ARB_imaging ) is supported )";
      break;
//    case GL_INVALID_FRAMEBUFFER_OPERATION:
//      errText = "GL_INVALID_FRAMEBUFFER_OPERATION";
//      break;
    default:
      return;

  }
  puts( errText );
}

int32_t MapInfo::getTileNumber( const Vec2 & worldPos )
{
  XY<int32_t> tileCoords = worldCoordToTileCoords( worldPos );
  tileCoords.x -= startX_px;
  tileCoords.y -= startY_px;
  return tileCoords.x + tileCoords.y * width;
}

int32_t MapInfo::setTileTexture( uint8_t texCoordSet, uint32_t atlasTextureNumber,
                                 const Vec2 & worldPos )
{
  XY<int32_t> tileCoords = worldCoordToTileCoords( worldPos );
  tileCoords.x -= startX_px;
  tileCoords.y -= startY_px;

  int32_t tileNr = tileCoords.x + tileCoords.y * width;
  float x = tileCoords.x;
  float y = tileCoords.y;
  MapVertex mv[4];
  const Vec2Quad & vq = atlasInfo->getTileUV( atlasTextureNumber );
  fillQuad( mv, x, y, vq, vq );

  uint32_t offset = tileNr * 4 * vbo.getVertexSize();
  uint32_t texCoordSize = 0;
  switch( texCoordSet ) {
    case 0:
      offset += MapVertex::TEX_OFFSET;
      texCoordSize = MapVertex::TEX_0_COUNT *
                     GlTypeToC<MapVertex::TEX_0_TYPE>::Size;

      tile[tileNr].first = atlasTextureNumber;
      break;
    case 1:
      offset += MapVertex::TEX_OFFSET +
                MapVertex::TEX_0_COUNT * GlTypeToC<MapVertex::TEX_0_TYPE>::Size;
      texCoordSize = MapVertex::TEX_1_COUNT *
                     GlTypeToC<MapVertex::TEX_1_TYPE>::Size;

      tile[tileNr].second = atlasTextureNumber;
      break;
    default:
      PRINT_ERROR( "nie ma takiego zestawu texcoordow w mapie\n" );
      return -1;
      break;
  }

  vbo.bind();

  const uint32_t vertSize = vbo.getVertexSize();
  if( offset >= vbo.capacity()*vertSize ) {     //bufor mniejszy niz offset więc nie ma co wrzucać
    PRINT_ERROR( "bufor mniejszy niz offset" );
    return -1 ;
  }
  glBufferSubData( GL_ARRAY_BUFFER, offset + 0 * vertSize, texCoordSize, &mv[0].t0 );
  checkPrintGlError();
  glBufferSubData( GL_ARRAY_BUFFER, offset + 1 * vertSize, texCoordSize, &mv[1].t0 );
  checkPrintGlError();
  glBufferSubData( GL_ARRAY_BUFFER, offset + 2 * vertSize, texCoordSize, &mv[2].t0 );
  checkPrintGlError();
  glBufferSubData( GL_ARRAY_BUFFER, offset + 3 * vertSize, texCoordSize, &mv[3].t0 );
  checkPrintGlError();

  vbo.unbind();
  return 0;
}

void MapInfo::saveMapText()
{
  saveMapText( mapName.c_str() );
}

void MapInfo::saveMapText( const char * filename )
{
  FILE * file = fopen( filename, "w" );
  assert( file );
  fprintf( file, "%i %i %i\n", startX_px / MapCmp::SCALE_X, startY_px / MapCmp::SCALE_Y,
           startZ_px );
  fprintf( file, "%i %i\n", width, height );
  fprintf( file, "%s\n", pngName.c_str() );
  fprintf( file, "%s\n", taiName.c_str() );
  for( int y = 0; y < ( int )height; ++y ) {
    for( int x = 0; x < ( int )width; ++x ) {
      fprintf( file, "%u %u, ", tile[x + y*width].first, tile[x+y*width].second );
    }/*koniec for (x)*/
    fputc( '\n', file );
  }/*koniec for (y)*/

  fclose( file );
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

  vShader = createAndCompileShader("shader/map.vert",GL_VERTEX_SHADER);
  fShader = createAndCompileShader("shader/map.frag",GL_FRAGMENT_SHADER);

  program = glCreateProgram();

  glAttachShader( program, vShader );
  glAttachShader( program, fShader );

  glLinkProgram( program );

  printShaderInfoLog( vShader );
  printShaderInfoLog( fShader );
  printProgramInfoLog( program );
}

MapCmp::~MapCmp()
{
  glDetachShader( program, vShader );
  glDetachShader( program, fShader );

  glDeleteShader( fShader );
  glDeleteShader( vShader );

  glDeleteProgram( program );
}

void MapCmp::draw( const RenderVec2 & rv2 )
{
//  glEnable( GL_BLEND );
//  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glUseProgram( program );
  indexVbo.bind();
  indexVbo.prepareDraw();
  for( int i = 0; i < ( int )records.size(); ++i ) {
    MapInfo * mi = records[i];
    //AtlasInfo * ai = game->getRender()->getAtlas(mi->)
    mi->vbo.bind();
    mi->vbo.prepareDraw();

    if(!(paneToDraw & 1)) {
      glClientActiveTexture( GL_TEXTURE0 );
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      //glEnable( GL_TEXTURE_2D );
      //glBindTexture( GL_TEXTURE_2D, mi->tex );
    }else{
      glClientActiveTexture( GL_TEXTURE0 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, mi->tex );
    }
    if(! (paneToDraw & 2) ){
      glClientActiveTexture( GL_TEXTURE1 );
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      /*
      glClientActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, mi->tex );
      */
    }else{
      glClientActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, mi->tex );
    }

    glDrawRangeElements( GL_TRIANGLES, 0, indexVbo.capacity(),
                         mi->width * mi->height * 6, GL_UNSIGNED_SHORT, 0 );
    mi->vbo.afterDraw();
    mi->vbo.unbind();
  }/*koniec for (i)*/
  indexVbo.afterDraw();
  indexVbo.unbind();

  //glDisable( GL_BLEND );
  glUseProgram( 0 );  //olaczam fixed pipeline
}

/*! zrwaca sektor mapy który obejmuje swoją powierzchnią parametr worldPos
*/
MapInfo * MapCmp::getSector( const Vec2 & worldPos )
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    MapInfo * mp = records[i];
    if( mp->startX_px <= worldPos.x &&
        mp->startX_px + mp->width * SCALE_X > worldPos.x ) {
      //x wiekszy od poczatku ale mniejszy od konca
      if( mp->startY_px <= worldPos.y &&
          mp->startY_px + mp->height * SCALE_Y > worldPos.y ) {
        return mp;
      }
    }
  }/*koniec for (i)*/
  return 0;
}

void MapCmp::saveAllText()
{
  for( int i = 0; i < ( int )records.size() ; ++i ) {
    records[i]->saveMapText( );
  }
}


void MapCmp::saveAllText( const char * baseName, const char * ext )
{
  std::string name;
  char number[20];
  for( int i = 0; i < ( int )records.size() ; ++i ) {
    name = baseName;
    name += "_";
    memset( number, 0, sizeof( number ) );
    sprintf( number, "%i", i );
    name += number;
    name += ext;

    records[i]->saveMapText( name.c_str() );
  }/*koniec for (i)*/
}

void MapCmp::drawOnlyPaneNr(int32_t pane)
{
  paneToDraw = pane;
}
