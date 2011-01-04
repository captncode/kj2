#include "precompiled.h"
#include "render.h"
#include "vbo.h"
#include "tai_loader.h"
#include "external/strnatcmp.h"
#include "main.h"

#undef EXTERNFLAG
#define EXTERNFLAG
#include "external/gl3header.h"


const Vec2Quad Render::fullTexture(
  RenderVec2( 0.f, 0.f ),             RenderVec2( 1.f, 0.f ),
  RenderVec2( 0.f, 1.f ),             RenderVec2( 1.f, 1.f )
);

const uint32_t      Render::white = 0xffffffff;
const float         Render::inv255 = 1.f / 255.f;
const AtlasInfo     Render::atlasNotFound ;

//dwa mega w bajtach
const uint32_t twoMegaByte = 2097152; //2*1024*1024;
const uint32_t oneMegaByte = 1048576;

//! \fn taiNaturalStringComp
/*! */
bool taiNaturalStringCompLess( const TaiLineData & l, const TaiLineData & r )
{
  return ( strnatcmp( l.filename, r.filename ) < 0 );
}//koniec funkcji taiNaturalStringComp

Vec2Quad AtlasInfo::getTileUV( const char * textureName )  const
{
  const uint32_t ind = getTextureNumber( textureName );
  return getTileUV( ind );
}

uint32_t AtlasInfo::getTextureNumber( const char * textureName ) const
{
  std::vector< TaiLineData >::const_iterator it =
    std::find_if( textureInfo.begin(), textureInfo.end(),
                  TaiFilenameComp( textureName ) );
  if(it == textureInfo.end() )
    return 0; //pierwsza teksturą z atlasu jest cały atlas

  return it - textureInfo.begin();
}
XY<uint32_t> AtlasInfo::getTextureSizePx( uint32_t index ) const
{
  return XY<uint32_t>((( float )size.x ) * textureInfo[index].width,
                      (( float )size.y ) * textureInfo[index].height );
}

/*! kod z: http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL
    postanowiłem niebawić sie w auxDIBImageLoad - narazie jest SDL

    rownie ciekawy link:
http://gpwiki.org/index.php/OpenGL:Tutorials:Tutorial_Framework:Texture_Mapping
*/
int32_t surfaceToTexture( SDL_Surface * inSurf, GLuint * out_tex )
{
  if( !inSurf )
    return ERROR_CODE;

  if(( inSurf->w & ( inSurf->w - 1 ) ) != 0 )
    return ERROR_CODE;// szerokość nie jest potęgą dwojki
  if(( inSurf->h & ( inSurf->h - 1 ) ) != 0 )
    return ERROR_CODE;//wysokosc nie jest potega dwojki

  uint32_t colCount = inSurf->format->BytesPerPixel;
  uint32_t format = 0;

  if( colCount == 4 ) {
    if( inSurf->format->Rmask == 0x000000ff )
      format = GL_RGBA;
    else
      format = GL_BGRA;
  }
  else if( colCount == 3 ) {  // bez alphy
    if( inSurf->format->Rmask == 0x000000ff )
      format = GL_RGB;
    else
      format = GL_BGR;
  }
  else {
    return ERROR_CODE;
  }

  glGenTextures( 1, out_tex );

  glBindTexture( GL_TEXTURE_2D, *out_tex );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  // Zapis koloru do textury
  /*
  void glTexImage2D(      GLenum target,
                          GLint   level,              level-of-detail
                          GLint   internalFormat,
                          GLsizei   width,
                          GLsizei   height,
                          GLint   border,             szerokosc ramki
                          GLenum    format,         format pixela
                          GLenum    type,
                          const GLvoid *    data    );
  */
  glTexImage2D( GL_TEXTURE_2D, 0, colCount, inSurf->w, inSurf->h,
                0, format, GL_UNSIGNED_BYTE, inSurf->pixels );

  glBindTexture( GL_TEXTURE_2D, 0 );

  return OK_CODE;
}

SpriteDef::SpriteDef() : shape(), atlasFile( "img/a1.png" )
  , atlasInfoFile( "img/a1.tai" )
  , textureName( "blank.png" ) , coordSpace( Render::SCREEN_COORD )
  , color( makeARGB( 255, 255, 255, 255 ) )
{}

SpriteInfo::SpriteInfo( const SpriteDef & def, Entity e, Game * game ):
  SpriteDef( def )
/*
atlasFile(def.atlasFile), atlasInfoFile(def.atlasInfoFile)
,textureName(def.textureName),coordSpace(def.coordSpace)
,color(def.color), depth(def.depth)
*/
  , tex(), textureNumber()
{
  Render * r = game->getRender();
  const AtlasInfo * ai =
    r->getAtlas( def.atlasFile.c_str(), def.atlasInfoFile.c_str() );
  if( !ai ) {
    ai = r->loadAtlas( def.atlasFile.c_str(), def.atlasInfoFile.c_str() );
    if( !ai ) PRINT_ERROR( "ERROR: nie zaladowano atlasu" );
  }
  tex = ai->tex;
  textureNumber = ai->getTextureNumber( def.textureName.c_str() );
}

SpriteInfo::SpriteInfo( char * line[] )
{
  entity = readEntityFormText(line[0] );
  shape = readEntityFormText(line[1]);

  char tmp[256] = {};
  sscanf( line[2], "%s", tmp );
  atlasFile = tmp;
  memset( tmp, 0, sizeof(tmp) );

  sscanf( line[3], "%s", tmp );
  atlasInfoFile = tmp;
  memset( tmp, 0, sizeof(tmp) );

  sscanf( line[4], "%s", tmp );
  textureName = tmp;
  //memset(tmp,0,256);

  memset( tmp, 0, 250 );
  sscanf( line[5], "%s", tmp );
  if( strcmp( tmp, "world" ) == 0 )
  {
    coordSpace = Render::WORLD_COORD;
  }
  else {
    coordSpace = Render::SCREEN_COORD;
  }

  sscanf( line[6], "%x", &color );

  int16_t vis = 0;
  sscanf( line[7], "%hi", &vis);
  visible = vis;
}

std::string SpriteInfo::getAsString() const
{
  std::string out;
  char tmp [256] = {};
  sprintf( tmp, "%u SpriteInfo\n %u shapeID\n", entity.getId(),shape.getId() );
  out += tmp;
  memset( tmp, 0, 256 );

  sprintf( tmp, " %s atlasFile\n", atlasFile.c_str() );
  out += tmp;
  memset( tmp, 0, 256 );

  sprintf( tmp, " %s atlasInfoFile\n", atlasInfoFile.c_str() );
  out += tmp;
  memset( tmp, 0, 256 );

  sprintf( tmp, " %s textureName\n", textureName.c_str() );
  out += tmp;
  memset( tmp, 0, 256 );

  const char * textCoordSpace[] = {"screen", "world" };
  sprintf( tmp, " %s coordSpace\n", textCoordSpace[coordSpace] );
  out += tmp;
  memset( tmp, 0, 256 );

  sprintf( tmp, " %#x color\n", color );
  out += tmp;
  memset( tmp, 0, 256 );

  int16_t vis = visible;
  sprintf( tmp, " %hi visible\n", vis);
  out += tmp;
  //memset(tmp,0,256);

  return out;
}
void SpriteInfo::afterLoad(Game * game)
{
//sprawdzam czy sprite wpis ma jakiś kształt
  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * sh = shcmp->get( shape );
  if( !sh ) //nie dodano entity do komponentu Shape
  {
    const AtlasInfo* ai = game->getRender()->getAtlas(tex);
    //wczytuje rozmiar obrazka w pikselach
    ShapeDef scd;
    XY<uint32_t> sizePx = ai->getTextureSizePx( textureNumber );
    scd.rect.upLeft = Vec2( sizePx.x*-0.5f,   sizePx.y*-0.5f );
    scd.rect.upRight = Vec2( sizePx.x*0.5f,   sizePx.y*-0.5f );
    scd.rect.downLeft = Vec2( sizePx.x*-0.5f, sizePx.y*0.5f );
    scd.rect.downRight = Vec2( sizePx.x*0.5f, sizePx.y*0.5f );
    // //wczytuje jednak nie zapisuje
    //shcmp->add( e, scd )->entity.saveIt = false;
  }
}



SpriteCmp::SpriteCmp( Game * game_ ) : BaseComponent<SpriteInfo>( game_ )
{
  SpriteDef def;
  Render * r = game->getRender();
  const AtlasInfo * ai =
    r->getAtlas( def.atlasFile.c_str(), def.atlasInfoFile.c_str() );
  if( !ai ) {
    ai = r->loadAtlas( def.atlasFile.c_str(), def.atlasInfoFile.c_str() );
    if( !ai ) PRINT_ERROR( "ERROR: nie zaladowano atlasu" );
  }
  untexturedSprite.tex = ai->tex;
  untexturedSprite.textureNumber = ai->getTextureNumber( def.textureName.c_str() );
  untexturedSprite.coordSpace = def.coordSpace;
  untexturedSprite.color = def.color;
}

/*! jesli widoczny to go narysuje
*/
void SpriteCmp::draw( Entity e , const ShapeDef& shapeDef)
{
  SpriteInfo * spriteInfo = 0;
  if(e.getId() == 0)
  {
    spriteInfo = &untexturedSprite;
  }else{
    spriteInfo = BaseType::get( e );
    if( ! spriteInfo )  return;
  }
  Render * r = game->getRender();
  ShapeCmp * shapeCmp = game->getShapeCmp();

  if( spriteInfo->visible )
  {
    Vec2Quad shape;
    translateQuad( shapeDef.rect, shapeDef.pos, &shape );
    if(shapeDef.angle)
      rotateQuad(shape,shapeDef.angle,shapeDef.pos,&shape);
    r->drawAtlas( spriteInfo->tex,
                  spriteInfo->textureNumber, shape,
                  Render::CoordSpace_e( spriteInfo->coordSpace ),
                  spriteInfo->color,
                  shapeDef.depth );
  }
}

/*! zawsze naysuje
*/
void SpriteCmp::forceDraw( Entity e , const ShapeDef& shapeDef)
{
  SpriteInfo * spriteInfo = 0;
  if(e.getId() == 0){
    spriteInfo = &untexturedSprite;
  }else{
    spriteInfo = BaseType::get( e );
    if( ! spriteInfo )  return;
  }

  Render * r = game->getRender();
  ShapeCmp * shapeCmp = game->getShapeCmp();

  Vec2Quad shape;
  translateQuad( shapeDef.rect, shapeDef.pos, &shape );
  if(shapeDef.angle)
    rotateQuad(shape,shapeDef.angle,shapeDef.pos,&shape);
  r->drawAtlas( spriteInfo->tex,
                spriteInfo->textureNumber, shape,
                Render::CoordSpace_e( spriteInfo->coordSpace ),
                spriteInfo->color,
                shapeDef.depth );
}

/*! rysuje tylko widoczne
*/
void SpriteCmp::drawAll()
{
  Render * r = game->getRender();
  ShapeCmp * shapeCmp = game->getShapeCmp();

  FOR_ALL( records, it ) {
    SpriteInfo * scd = *it;
    ShapeDef * shapeDef = shapeCmp->get( scd->shape );
    if( scd->visible ) {

      Vec2Quad shape;
      //rotateQuad(shapeDef->rect,shapeDef->angle,shapeDef->pos,&shape );
      //translateQuad( shape, shapeDef->pos, &shape );
      translateQuad( shapeDef->rect, shapeDef->pos, &shape );
      if(shapeDef->angle)
        rotateQuad(shape,shapeDef->angle,shapeDef->pos,&shape);
      r->drawAtlas( scd->tex, scd->textureNumber, shape,
                    Render::CoordSpace_e( scd->coordSpace ), scd->color,
                    shapeDef->depth );
    }
  }//koniec for(records)
}
void SpriteCmp::setColor( Entity e, uint32_t color ) const
{
  SpriteInfo * si = BaseType::get( e );
  if( si )
    si->color = color;
}
void SpriteCmp::setVisibility ( Entity e, bool val) const
{
  SpriteInfo * si = BaseType::get( e );
  if(si){
    si->visible = val;
  }
}
const AtlasInfo * SpriteCmp::getAtlas(Entity e) const {
  return game->getRender()->getAtlas(getSure(e)->tex);
}

int32_t SpriteCmp::getTextureSizePx( Entity e ,XY<uint32_t>* out) const
{
  const AtlasInfo * ai = getAtlas(e);

  if(ai){
    *out = ai->getTextureSizePx(getSure(e)->textureNumber);
    return 0;
  }
  return -1;
}

void SpriteCmp::drawRect( const ShapeDef & d, uint32_t argb ) {
  Vec2Quad shape;
  translateQuad( d.rect, d.pos, &shape );
  rotateQuad(shape,d.angle,d.pos,&shape);
  game->getRender()->
  drawAtlas( untexturedSprite.tex,
             untexturedSprite.textureNumber, shape,
             Render::CoordSpace_e( untexturedSprite.coordSpace ),
             argb, d.depth );
}



TextInfo::TextInfo() : text(), position(), color( 0xffff00ff ), font( 0 )
  , coordSpace( Render::SCREEN_COORD ), depth( 1 ), visible( true )
{}

TextInfo::TextInfo( char * line[] )
{
  uint32_t e = 0;
  sscanf( line[0], "%u", &e);
  assert(e);
  entity = Entity (e);

  char * begin = strchr( line[1], '"' ) + 1;
  char * end = strchr( begin, '"' );
  text = begin;
  int l = 2;
  while( !end ) {
    text += '\n';
    end = strchr( line[l], '"' );
    text += line[l];
    ++l;
  }
  pop_back( text ); //usuwam ostatni nawias

  l -= 2;
  sscanf( line[l+2], "%f", &position.x );
  sscanf( line[l+3], "%f", &position.y );

  sscanf( line[l+4], "%x", &color );
  sscanf( line[l+5], "%u", &font );
  sscanf( line[l+6], "%u", &coordSpace );

  sscanf( line[l+7], "%hi", &depth );
  short a;
  sscanf( line[l+8], "%hi", &a );
  visible = a;
}

std::string TextInfo::getAsString()
{
  char tmp[250] = {};
  sprintf( tmp, "%i TextInfo entity\n ", entity.getId() );

  std::string out( tmp );
  out += "\"";
  out += text;
  out += "\"\n";

  memset( tmp, 0, 250 );
  sprintf( tmp, "%f position.x\n %f position.y\n ", position.x, position.y );

  out += tmp;
  memset( tmp, 0, 250 );
  sprintf( tmp, "%#x color\n %u font\n %u coordSpace\n %hi depth\n %hi visible\n",
           color, font, coordSpace, depth, ( short )visible );
  out += tmp;

  return out;
}
void TextInfo::afterLoad(Game * game)
{

}

TextCmp::TextCmp( Game * game_ ) : BaseType( game_ )
  , bitmapFont( 1, CBitmapFont( game->getRender() ) )
{
  if( !bitmapFont[0].Load( FONTS_PATCH "Consolas.bff" ) )
  {
    PRINT_ERROR( "nie odnaleziono czcionki Consolas.bff" );
  }
}

int32_t TextCmp::loadFont( const char * file ) {
  bitmapFont.push_back( CBitmapFont( game->getRender() ) );
  if( ! bitmapFont.back().Load( file ) ) {
    PRINT_ERROR( "nie odnaleziono czcionki: " );
    puts( file );
    bitmapFont.pop_back();
    return -1;
  }
  return bitmapFont.size() - 1;
}

void TextCmp::drawAll()
{
  Render * r = game->getRender();
  for( int i = 0; i < ( int )records.size(); ++i ) {
    TextInfo * ti = records[i];

    if( ti->visible && ti->font < bitmapFont.size() )
    {
      uint32_t & argb = ti->color;
      //bitmapFont[ti->font].SetColor( argb>>24 , argb>>16 , argb>>8, argb);
      bitmapFont[ti->font].Print( ti->text.c_str(), ti->position.x, ti->position.y,
                                  argb >> 24, argb >> 16 , argb >> 8,
                                  argb, ti->coordSpace, ti->depth );
    }
  }/*koniec for (i)*/
}
void TextCmp::draw( Entity e )
{
  Render * r = game->getRender();
  TextInfo * ti = get( e );
  if( ti->visible && ti->font < bitmapFont.size() )
  {
    uint32_t & argb = ti->color;
    //bitmapFont[ti->font].SetColor( argb>>24 , argb>>16 , argb>>8, argb);
    bitmapFont[ti->font].Print( ti->text.c_str(), ti->position.x, ti->position.y,
                                argb >> 24, argb >> 16 , argb >> 8,
                                argb, ti->coordSpace, ti->depth );
  }
}
int TextCmp::getTextWidth( const char * str, uint32_t font, uint32_t len ) const
{
  if( font < bitmapFont.size() ) {
    return bitmapFont[font].GetWidth( str, len );
  }
  return -1;
}

int TextCmp::getFontHeight( uint32_t font ) const
{
  if( font < bitmapFont.size() ) {
    return bitmapFont[font].GetHeight();
  }
  return -1;
}

Render::Render( Game * game_, const XY<uint32_t>& wndDim_, uint32_t depth_,
                bool fullscreen_ ) : game( game_ ), winDim( wndDim_ )
  , deskDim( 0, 0 ), colorDepth( depth_ ), zoom( 1.f ), invZoom( 1.f / zoom )
  , fullscreen( fullscreen_ )
  , wasinit( false )
  , screenOrig()
  , spriteVbo()
  , spriteIbo()
  , bitmapFont( 1, CBitmapFont( this ) )
{
  initOGL( winDim, colorDepth, fullscreen );
}

Render::~Render()
{
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    glDeleteTextures( 1, &it->tex );
  }//koniec for(atlas)
}

SDL_Surface * Render::initOGL( const XY<uint32_t>& wndDim, uint32_t colorDepth, bool fullscreen )
{
  this->winDim = wndDim;
  this->colorDepth = colorDepth;
  this->fullscreen = fullscreen;


  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    printf( "Unable to init SDL: %s\n", SDL_GetError() );
    ERROR_CODE;
    return 0;
  }

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );

  printf("GL_MAX_TEXTURE_COORDS: %i\n",GL_MAX_TEXTURE_COORDS);

#ifdef _DEBUG
  int ret = 0;
#define CHECK_FOR_GL_ATTRIB(attr)\
  if(!SDL_GL_GetAttribute(attr,&ret) ) printf("value of:%s\t %i\n",#attr,ret); \
  else puts("SDL_GL_GetAttribute error");

  CHECK_FOR_GL_ATTRIB( SDL_GL_RED_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_GREEN_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_BLUE_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ALPHA_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_BUFFER_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_DOUBLEBUFFER )
  CHECK_FOR_GL_ATTRIB( SDL_GL_DEPTH_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_STENCIL_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ACCUM_RED_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ACCUM_GREEN_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ACCUM_BLUE_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ACCUM_ALPHA_SIZE )
  CHECK_FOR_GL_ATTRIB( SDL_GL_STEREO )
  CHECK_FOR_GL_ATTRIB( SDL_GL_MULTISAMPLEBUFFERS )
  CHECK_FOR_GL_ATTRIB( SDL_GL_MULTISAMPLESAMPLES )
  CHECK_FOR_GL_ATTRIB( SDL_GL_SWAP_CONTROL )
  CHECK_FOR_GL_ATTRIB( SDL_GL_ACCELERATED_VISUAL )

#undef CHECK_FOR_GL_ATTRIB
#endif/*_DEBUG*/

  const SDL_VideoInfo * pVideo =  SDL_GetVideoInfo();
  if( pVideo ) {
    deskDim.x = pVideo->current_w;
    deskDim.y = pVideo->current_h;
  }
  /*! cytat z : http://www.libsdl.org/cgi/docwiki.cgi/SDL_SetVideoMode
        User note 2: Also note that, in Windows, setting the video mode resets the current
        OpenGL context. You must execute again the OpenGL initialization
        code (set the clear color or the shade model, or reload textures, for example)
        after calling SDL_SetVideoMode. In Linux, however, it works fine,
        and the initialization code only needs to be executed after the first
        call to SDL_SetVideoMode (although there is no harm in executing the
        initialization code after each call to SDL_SetVideoMode, for example
        for a multiplatform application).
    */
// TODO (boo#1#): przerobić to, rzobić wczytywanie configa z pliku
  SDL_Surface * screen = 0;
  if( !fullscreen ) {
    screen = SDL_SetVideoMode( wndDim.x, wndDim.y, colorDepth, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWSURFACE /* | SDL_RESIZABLE*/ );
    glViewport( 0, 0, wndDim.x, wndDim.y );
  }
  else {
    screen = SDL_SetVideoMode( deskDim.x, deskDim.y, colorDepth, SDL_OPENGL | SDL_FULLSCREEN );
    glViewport( 0, 0, deskDim.x, deskDim.y );
  }

  if( !screen ) {
    printf( "Unable to set init ogl %s\n", SDL_GetError() );
    ERROR_CODE;
    return 0;
  }
  printf( "ogl ver: %s\n\n", glGetString( GL_VERSION ) );
  puts( "supported ogl extensions:" );
  {
    char * str = ( char * )glGetString( GL_EXTENSIONS );
    char * ext;
    ext = strtok( str, " " );
    while( ext != NULL ) {
      printf( "%s\n", ext );
      ext = strtok( NULL, " " );
    }
  }

  //glInit:
  glClearColor( 0.f, 0.f, 0.f, 0.f ); //rgba: <0,1>

  glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );

  //Passes if the incoming depth value is less than or equal to the stored depth value.
  glDepthFunc( GL_LEQUAL );

  glEnable( GL_CULL_FACE );
  glCullFace( GL_BACK );
  glFrontFace( GL_CW );

#define GERPROCADDRESSPROC SDL_GL_GetProcAddress
#include "external/gl3loading.h"

  //narazie włączam 2 tekstury
  glClientActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);

  // wybiera drugą jednostkę tekstury
  glClientActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);


  // load support for the JPG and PNG image formats
  int flags =/*IMG_INIT_JPG|*/IMG_INIT_PNG;
  int initted = IMG_Init( flags );
  if(( initted & flags ) != flags ) {
    printf( "IMG_Init: Failed to init required png support!\n" );
    printf( "IMG_Init: %s\n", IMG_GetError() );
    ERROR_CODE;
    //return screen;
  }

  if( !bitmapFont[0].Load( FONTS_PATCH "Consolas.bff" ) ) {
    PRINT_ERROR( "nie odnaleziono czcionki Consolas.bff" );
  }

  spriteVbo = new Vbo<SpriteVert, GL_ARRAY_BUFFER>;
  spriteIbo = new Vbo<uint16_t, GL_ELEMENT_ARRAY_BUFFER>;

  const uint32_t RESERVED_SIZE_FOR_SPRITES = 15000;
  spriteVbo->bind();
  spriteVbo->reserve( RESERVED_SIZE_FOR_SPRITES, GL_STREAM_DRAW, 0 );
  spriteVbo->unbind();

  spriteIbo->bind();
  spriteIbo->reserve( RESERVED_SIZE_FOR_SPRITES, GL_STREAM_DRAW );

  //wypełniam cały bufor indexów wartościami umożliwiającyni rysowanie quadów,kwadratów
  uint16_t * mapedInd = spriteIbo->map( GL_WRITE_ONLY );
  for( int i = 0, index = 0; index < ( int )spriteIbo->capacity(); ++i ) {
    mapedInd[index++] = 0 + i * 4;
    mapedInd[index++] = 1 + i * 4;
    mapedInd[index++] = 2 + i * 4;

//    mapedInd[index++] = 1 + i * 4;
//  	mapedInd[index++] = 3 + i * 4;
//  	mapedInd[index++] = 2 + i * 4;

    mapedInd[index++] = 0 + i * 4;
    mapedInd[index++] = 2 + i * 4;
    mapedInd[index++] = 3 + i * 4;
  }//koniec for (i)
  spriteIbo->unmap();
  spriteIbo->unbind();

  wasinit = true;
  return screen;
}

void Render::shutdownOGL()
{
  delete spriteVbo;
  delete spriteIbo;

  printf( "Exited cleanly\n" );
  IMG_Quit();
  SDL_Quit();
}

uint32_t Render::beginDraw( const RenderVec2 & centre )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  setScrCentre( centre );
  return OK_CODE;
}

uint32_t Render::endDraw()
{
  sortAndDrawSprites();
  SDL_GL_SwapBuffers();
  return OK_CODE;
}

void Render::sortAndDrawSprites()
{
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  if( spriteToDraw.size() > 0 )
  {
    std::stable_sort( spriteToDraw.begin(), spriteToDraw.end(), byDepth );

    spriteVbo->bind();
    spriteVbo->discard();
    spriteVbo->setInserter( 0 );
    for( __typeof( spriteToDraw.begin() ) it = spriteToDraw.begin(); it != spriteToDraw.end(); ++it ) {
      SpriteRenderInfo & sri = *it;

      //glBindTexture(GL_TEXTURE_2D,sri.tex);
      spriteVbo->add( 4, sri.v );

    }//koniec for(spriteToDraw)

    spriteIbo->bind();
    spriteIbo->prepareDraw();

    spriteVbo->prepareDraw();
    GLuint prevTex = 0;
    for( int i = 0; i < ( int )spriteToDraw.size(); ++i ) {
      if( spriteToDraw[i].tex != prevTex ) {
        glBindTexture( GL_TEXTURE_2D, spriteToDraw[i].tex );
        prevTex = spriteToDraw[i].tex;
      }

      //rysuje jednego sprite'a
      glDrawRangeElements( GL_TRIANGLES, i * 6, ( i + 1 ) * 6 + 1, 6,
                           GL_UNSIGNED_SHORT,
                           BUFFER_OFFSET( i * 6 * sizeof( uint16_t ) ) );
    }//for i
    spriteVbo->afterDraw();
    spriteVbo->unbind();

    spriteIbo->afterDraw();
    spriteIbo->unbind();

    spriteToDraw.clear();
  }


  //glBindTexture( GL_TEXTURE_2D, 0 );
  glDisable( GL_BLEND );
}

void Render::setScrCentre( const RenderVec2 & so )
{
  //screenOrig = ( -0.5f * invZoom * RenderVec2( winDim.x, winDim.y ) ) + so;
  screenOrig = RenderVec2(( -0.5f * invZoom * (( float )winDim.x ) ) + so.x,
                          ( -0.5f * invZoom * (( float )winDim.y ) ) + so.y );


  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  // L/R/B/T
  glOrtho(( winDim.x * -0.5f )*invZoom + so.x,
          ( winDim.x * 0.5f )*invZoom + so.x,
          ( winDim.y * 0.5f )*invZoom + so.y,
          ( winDim.y * -0.5f )*invZoom + so.y, -1.0, 1.0 );

  glMatrixMode( GL_MODELVIEW );
}

void Render::setScrOrig( const RenderVec2 & so )
{
  //screenOrig = so ;
  screenOrig = so * invZoom;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( so.x, so.x + winDim.x * invZoom, so.y + winDim.y * invZoom, so.y,
           -1.0, 1.0 );

  glMatrixMode( GL_MODELVIEW );
}

const AtlasInfo * Render::loadAtlas( const char * name, const char * taiName )
{
  AtlasInfo atlasInfo;
  atlasInfo.filename = name;
  atlasInfo.infoFilename = taiName;

  GLuint tex = 0;
  SDL_Surface * pSurf = IMG_Load( name );
  PRINT_ERROR( " ladowanie atlasu: " );
  DEBUG_PRINTF( name, "%s\t" );
  DEBUG_PRINTF( pSurf, "%p\n" );
  assert( pSurf && name );
  atlasInfo.size.x = pSurf->w;
  atlasInfo.size.y = pSurf->h;

  surfaceToTexture( pSurf, &tex );
  atlasInfo.tex = tex;

  SDL_FreeSurface( pSurf );

  TaiLoader taiLoader( taiName );
  TaiLineData tmp_tld;
  atlasInfo.textureInfo.push_back(tmp_tld);

  while( taiLoader.read( &tmp_tld ) ) {
    atlasInfo.textureInfo.push_back( tmp_tld );
  }
  strncpy(tmp_tld.filename,name,100);
  tmp_tld.height = 1.f;
  tmp_tld.width = 1.f;
  tmp_tld.hoffset = 0.f;
  tmp_tld.woffset = 0.f;
  atlasInfo.textureInfo[0] = tmp_tld;


  //std::sort( atlasInfo.textureInfo.begin(), atlasInfo.textureInfo.end(),
  //           taiNaturalStringCompLess );

  atlas.push_back( atlasInfo );
  return &atlas.back();
}

void Render::unload( GLuint * tex )
{
  glDeleteTextures( 1, tex );
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    if( it->tex == *tex ) {
      atlas.erase( it );
      glDeleteTextures( 1, tex );
      return;
    }
  }//koniec for(atlas)
}

int32_t Render::drawSprite( GLuint texture, const Vec2Quad & uv,
                            const Vec2Quad & pos, const CoordSpace_e space,
                            uint32_t color, int16_t depth  )
{
  float zoomFactor = 1.f;
  //float zoomFactor = invZoom;
  RenderVec2 orig( 0.f, 0.f );
  if( SCREEN_COORD == space ) {
    zoomFactor = invZoom;
    orig = screenOrig;
  }
  static float depthToFloat = -1.f / float(( 1 << 16 ) - 1 );

  SpriteRenderInfo toDraw;
  toDraw.tex = texture;
  const RenderVec2 * posV;
  const RenderVec2 * texV;
  //zle to kopiowanie wygląda ale moznaby to załatwić przez memcpy
  for( int i = 0; i < ( int )4; ++i ) {
    posV = &pos.at( i );
    texV = &uv.at( i );
    toDraw.v[i].x = posV->x * zoomFactor + orig.x ;
    toDraw.v[i].y = posV->y * zoomFactor + orig.y ;
    toDraw.v[i].z = depth * depthToFloat;

    toDraw.v[i].tx = texV->x;
    toDraw.v[i].ty = texV->y;

    toDraw.v[i].r = uint8_t( color >> 16 );
    toDraw.v[i].g = uint8_t( color >> 8 );
    toDraw.v[i].b = uint8_t( color );
    toDraw.v[i].a = uint8_t( color >> 24 );

  }//koniec for (i)

  spriteToDraw.push_back( toDraw );

  return 0;
}

int32_t Render::drawAtlas( GLuint texture, uint32_t tileIndex,
                           const Vec2Quad & pos, const CoordSpace_e space,
                           uint32_t color, int16_t depth )
{
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    if( texture == it->tex ) {
      const Vec2Quad & uv = it->getTileUV( tileIndex );
      drawSprite( texture, uv, pos, space, color , depth );
      return OK_CODE;
    }
  }//koniec for(atlas)
  PRINT_ERROR( "nie znaleziono textury\n" );
  return ERROR_CODE;
}

const AtlasInfo * Render::getAtlas( const char * filename )
{
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    if( !strcmp( it->filename.c_str(), filename ) )
      return &( *it );
  }//koniec for(atlas)
  PRINT_ERROR( "nie znaleziono atlasu: " );
  fputs( filename, stdout );
  fputs( "\n", stdout );
  return 0;
}

const AtlasInfo * Render::getAtlas( const char * filename,
                                    const char * infoFilename )
{
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    if( !strcmp( it->filename.c_str(), filename ) &&
        !strcmp( it->infoFilename.c_str(), infoFilename ) )
      return &( *it );
  }//koniec for(atlas)
  PRINT_ERROR( "nie znaleziono atlasu: " );
  fputs( filename, stdout );
  fputs( "; ", stdout );
  puts( infoFilename );
  return 0;
}

