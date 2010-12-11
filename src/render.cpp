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
  if( ind != textureInfo.size() )
    return getTileUV( ind );

  return Render::fullTexture;
}

uint32_t AtlasInfo::getTextureNumber( const char * textureName ) const
{
//    char buffer[250];
//    int len = strlen(baseName);
//    strcpy(buffer,baseName);
//    buffer[len] = '/';
//    strcat(buffer+len+1,textureName);
//    puts(buffer);
  std::vector< TaiLineData >::const_iterator it =
    std::find_if( textureInfo.begin(), textureInfo.end(), TaiFilenameComp( textureName ) );

  return it - textureInfo.begin();
  //return std::distance( textureInfo.begin(), it );
}

/*! kod z: http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL
    postanowiłem niebawić sie w auxDIBImageLoad - narazie jest SDL

    rownie ciekawy link: http://gpwiki.org/index.php/OpenGL:Tutorials:Tutorial_Framework:Texture_Mapping
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

SpriteInfo::SpriteInfo(const SpriteDef& def,Entity e, Game * game):
  entity(e),coordSpace(def.coordSpace),color(def.color)
{
  Render * r= game->getRender();
  const AtlasInfo * ai =
    r->getAtlas( def.atlasFile.c_str(),def.atlasInfoFile.c_str() );
  if( !ai ){
    ai = r->loadAtlas(def.atlasFile.c_str(),def.atlasInfoFile.c_str() );
    if(!ai) PRINT_ERROR("ERROR: nie zaladowano atlasu");
  }
  tex = ai->tex;
  textureNumber = ai->getTextureNumber(def.textureName.c_str() );

  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * sh = shcmp->get(e);
  if(!sh)   //nie dodano entity do komponentu Shape
  {
    ShapeDef scd;
    shcmp->add(e,scd);
  }
}
void SpriteCmp::draw()
{
  Render * r = game->getRender();
  ShapeCmp * sc = game->getShapeCmp();

  FOR_ALL(records,it){
  	SpriteInfo * scd = *it;
  	ShapeDef * shapecd = sc->get(scd->entity);

    Vec2Quad shape;
    translateQuad(shapecd->rect,shapecd->pos,&shape);
    r->drawAtlas(scd->tex,scd->textureNumber,shape,
                 Render::CoordSpace_e(scd->coordSpace),scd->color,
                 shapecd->depth);

  }//koniec for(records)
}
void SpriteCmp::setColor(Entity e, uint32_t color)
{
  SpriteInfo * si = get(e);
  if(si)
    si->color = color;
}

TextInfo::TextInfo() : position(),color(0xffaaaaaa),font(0)
  ,coordSpace(Render::SCREEN_COORD),depth(0)
{}

TextCmp::TextCmp(Game * game_) : BaseComponent<TextInfo>(game_)
  ,bitmapFont(1,CBitmapFont(game->getRender() ) )
{
  if( !bitmapFont[0].Load( FONTS_PATCH "Consolas.bff" ) )
  {
    PRINT_ERROR("nie odnaleziono czcionki Consolas.bff");
  }
}

int32_t TextCmp::loadFont( const char * file ) {
  bitmapFont.push_back( CBitmapFont( game->getRender() ) );
  if( ! bitmapFont.back().Load( file ) ) {
    PRINT_ERROR( "nie odnaleziono czcionki: ");
    puts(file);
    bitmapFont.pop_back();
    return -1;
  }
  return bitmapFont.size() - 1;
}

void TextCmp::draw()
{
  Render * r = game->getRender();
  for(int i = 0; i < (int)records.size(); ++i ){
    TextInfo * ti = records[i];

    if(ti->font < bitmapFont.size() )
    {
      uint32_t & argb = ti->color;
      //bitmapFont[ti->font].SetColor( argb>>24 , argb>>16 , argb>>8, argb);
      bitmapFont[ti->font].Print(ti->text.c_str(),ti->position.x,ti->position.y,
                                 argb>>24,argb>>16 , argb>>8,
                                 argb,ti->coordSpace, ti->depth );
    }
  }/*koniec for (i)*/
}

//void TextCmp::setPosition(Entity e, RenderVec2 pos)
//{
//  for(int i = 0; i < (int)records.size(); ++i ){
//    if( e == records[i]->entity){
//      records[i]->position = pos;
//    }
//  }/*koniec for (i)*/
//}



Render::Render( Game * game_,const XY<uint32_t>& wndDim_, uint32_t depth_,
                bool fullscreen_) : game( game_ ), winDim( wndDim_)
  , deskDim( 0, 0 ), depth( depth_ ), zoom( 32.f ), invZoom( 1.f / zoom )
  , fullscreen(fullscreen_)
  , wasinit( false )
  , screenOrig()
  , spriteVbo()
  , spriteIbo()
  , bitmapFont( 1, CBitmapFont( this ) )
{
  initOGL(winDim,depth,fullscreen);
}

Render::~Render()
{
  for( __typeof( atlas.begin() ) it = atlas.begin(); it != atlas.end(); ++it ) {
    glDeleteTextures( 1, &it->tex );
  }//koniec for(atlas)
}

SDL_Surface * Render::initOGL( const XY<uint32_t>& wndDim, uint32_t depth, bool fullscreen )
{
  this->winDim = wndDim;
  this->depth = depth;
  this->fullscreen = fullscreen;


  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    printf( "Unable to init SDL: %s\n", SDL_GetError() );
    ERROR_CODE;
    return 0;
  }

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 );

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
    screen = SDL_SetVideoMode( wndDim.x, wndDim.y, depth, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWSURFACE /* | SDL_RESIZABLE*/ );
    glViewport( 0, 0, wndDim.x, wndDim.y );
  }
  else {
    screen = SDL_SetVideoMode( deskDim.x, deskDim.y, depth, SDL_OPENGL | SDL_FULLSCREEN );
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

  glEnable( GL_TEXTURE_2D );


#define GERPROCADDRESSPROC SDL_GL_GetProcAddress
#include "external/gl3loading.h"

  // load support for the JPG and PNG image formats
  int flags =/*IMG_INIT_JPG|*/IMG_INIT_PNG;
  int initted = IMG_Init( flags );
  if(( initted & flags ) != flags ) {
    printf( "IMG_Init: Failed to init required png support!\n" );
    printf( "IMG_Init: %s\n", IMG_GetError() );
    ERROR_CODE;
    //return screen;
  }

  if( !bitmapFont[0].Load( FONTS_PATCH "Consolas.bff" ) ){
    PRINT_ERROR("nie odnaleziono czcionki Consolas.bff");
  }

  spriteVbo = new Vbo<SpriteVert, GL_ARRAY_BUFFER>;
  spriteIbo = new Vbo<uint16_t, GL_ELEMENT_ARRAY_BUFFER>;

  const uint32_t RESERVED_SIZE_FOR_SPRITES = 1024;
  spriteVbo->bind();
  spriteVbo->reserve(RESERVED_SIZE_FOR_SPRITES,GL_STREAM_DRAW,0);
  spriteVbo->unbind();

  spriteIbo->bind();
  spriteIbo->reserve( RESERVED_SIZE_FOR_SPRITES, GL_STREAM_DRAW );

  //wypełniam cały bufor indexów wartościami umożliwiającyni rysowanie quadów,kwadratów
  uint16_t * mapedInd = spriteIbo->map(GL_WRITE_ONLY);
  for(int i = 0,index = 0; index < (int)spriteIbo->capacity(); ++i ){
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
  delete spriteVbo;

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

  if(spriteToDraw.size() > 0)
  {
    std::sort(spriteToDraw.begin(),spriteToDraw.end(),byTexture);
    uint32_t lastTexChange = 0; //w ktorym obiegu ostatnio zmieniła sie textura

    //sortuje teraz podciągi według głębokości
    for(int i = 1; i < (int)spriteToDraw.size(); ++i ){
      if(spriteToDraw[i].tex != spriteToDraw[i-1].tex ) //zmiana tekstury
      { //narazie wszystkie 4 wierzchołki mają taką samą głębokość
        std::sort( spriteToDraw.begin()+ lastTexChange,
                  spriteToDraw.begin()+ i-1, byDepth);

        lastTexChange = i;
      }
    }/*koniec for (i)*/
    {
      int i = spriteToDraw.size();
      std::sort( spriteToDraw.begin()+ lastTexChange,
              spriteToDraw.begin()+ i-1, byDepth);
    }


    spriteVbo->bind();
    spriteVbo->discard();
    spriteVbo->setInserter(0);
    for( __typeof(spriteToDraw.begin()) it = spriteToDraw.begin(); it != spriteToDraw.end(); ++it ){
      SpriteRenderInfo& sri = *it;

      //glBindTexture(GL_TEXTURE_2D,sri.tex);
      spriteVbo->add(4,sri.v );

    }//koniec for(spriteToDraw)
    spriteIbo->bind();
    spriteIbo->prepareDraw();

    lastTexChange = 0; //w ktorym obiegu ostatnio zmieniła sie textura
    spriteVbo->prepareDraw();
    for(int i = 1; i < (int)spriteToDraw.size(); ++i ){
      if(spriteToDraw[i].tex != spriteToDraw[i-1].tex )
      {//zmieniła się textura,atlas - trzeba rysować
        const uint32_t minInd = lastTexChange * 4;
        const uint32_t maxInd = i * 4 - 1;
        glBindTexture(GL_TEXTURE_2D,spriteToDraw[i-1].tex);

        glDrawRangeElements(GL_TRIANGLES,minInd,maxInd,( i-lastTexChange )*6,
                            GL_UNSIGNED_SHORT,
                            BUFFER_OFFSET( lastTexChange*6*sizeof(uint16_t) ) );

        lastTexChange = i;
      }
    }//koniec for (i)
    {
      int i = spriteToDraw.size();
      const uint32_t minInd = lastTexChange * 4;
      const uint32_t maxInd = i * 4 - 1;
      glBindTexture(GL_TEXTURE_2D,spriteToDraw[i-1].tex);
      glDrawRangeElements(GL_TRIANGLES,minInd,maxInd,( i-lastTexChange )*6,
                          GL_UNSIGNED_SHORT,
                          BUFFER_OFFSET( lastTexChange*6*sizeof(uint16_t) ) );
    }
    spriteVbo->afterDraw();
    spriteVbo->unbind();

    spriteIbo->afterDraw();
    spriteIbo->unbind();

    spriteToDraw.clear();
  }//koniec if(spriteToDraw.size() > 0)


  glBindTexture(GL_TEXTURE_2D,0);
  glDisable( GL_BLEND );
}

void Render::setScrCentre( const RenderVec2 & so )
{
  //screenOrig = ( -0.5f * invZoom * RenderVec2( winDim.x, winDim.y ) ) + so;
  screenOrig = RenderVec2( (-0.5f * invZoom * ((float)winDim.x)) + so.x,
                          (-0.5f * invZoom * ((float)winDim.y)) + so.y);
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
  screenOrig = so;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( so.x, so.x + winDim.x * invZoom, so.y + winDim.y * invZoom, so.y, -1.0, 1.0 );
  glMatrixMode( GL_MODELVIEW );
}

const AtlasInfo * Render::loadAtlas( const char * name, const char * taiName )
{
  AtlasInfo atlasInfo;
  atlasInfo.filename = name;
  atlasInfo.infoFilename = taiName;

  GLuint tex = 0;
  SDL_Surface * pSurf = IMG_Load( name );
  PRINT_ERROR(" ladowanie atlasu: ");
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

  while( taiLoader.read( &tmp_tld ) ) {
    atlasInfo.textureInfo.push_back( tmp_tld );
  }

  std::sort( atlasInfo.textureInfo.begin(), atlasInfo.textureInfo.end(),
             taiNaturalStringCompLess );

  atlas.push_back( atlasInfo );
  return &atlas.back();
}

void Render::unload( GLuint * tex )
{
  glDeleteTextures( 1, tex );
  for( __typeof(atlas.begin()) it = atlas.begin(); it != atlas.end(); ++it ){
  	if(it->tex == *tex){
      atlas.erase(it);
      glDeleteTextures( 1, tex );
      return;
  	}
  }//koniec for(atlas)
}

int32_t Render::drawSprite( GLuint texture,const Vec2Quad &uv,
                              const Vec2Quad &pos, const CoordSpace_e space,
                              uint32_t color,int16_t depth )
{
  float zoomFactor = 1.f;
  RenderVec2 orig( 0.f, 0.f );
  if( SCREEN_COORD == space ) {
    zoomFactor = invZoom;
    orig = screenOrig;
  }
  static float depthToFloat = 1.f / std::numeric_limits<int16_t>::max();

  SpriteRenderInfo toDraw;
  toDraw.tex = texture;
  const RenderVec2 * posV;
  const RenderVec2 * texV;
  //zle to kopiowanie wygląda ale moznaby to załatwić przez memcpy
  for( int i = 0; i < ( int )4; ++i ) {
    posV = &pos.at(i);
    texV = &uv.at(i);
    toDraw.v[i].x = posV->x * zoomFactor + orig.x;
    toDraw.v[i].y = posV->y * zoomFactor + orig.y;
    toDraw.v[i].z = depth * depthToFloat;

    toDraw.v[i].tx = texV->x;
    toDraw.v[i].ty = texV->y;

    toDraw.v[i].r = uint8_t(color>>16);
    toDraw.v[i].g = uint8_t(color>>8 );
    toDraw.v[i].b = uint8_t(color);
    toDraw.v[i].a = uint8_t(color>>24);

  }//koniec for (i)

  spriteToDraw.push_back( toDraw );

  return 0;
}

int32_t Render::drawAtlas( GLuint texture,uint32_t tileIndex,
                            const Vec2Quad &pos, const CoordSpace_e space,
                            uint32_t color, int16_t depth )
{
  for( __typeof(atlas.begin()) it = atlas.begin(); it != atlas.end(); ++it ){
  	if(texture == it->tex){
      const Vec2Quad &uv = it->getTileUV( tileIndex );
      drawSprite( texture, uv, pos, space, color , depth);
      return OK_CODE;
  	}
  }//koniec for(atlas)
  PRINT_ERROR("nie znaleziono textury");
  return ERROR_CODE;
}
