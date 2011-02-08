#include "precompiled.h"
#include "drawGui.h"

#include "widget.h"
#include "guicmp.h"
#include "motioncmp.h"
#include "inputcmp.h"
#include "render.h"


uint32_t sliderXf( GuiInfo * guiInfo, float percent )
{
  assert( percent <= 100 );

  const Entity & e = guiInfo->entity;

  SpriteCmp * spriteCmp = g_game->getSpriteCmp();
  GuiCmp * guiCmp = g_game->getGuiCmp();
  //GuiInfo * guiInfo = g_game->getGuiCmp()->getSure( e );
  ShapeDef * shapeDef = guiInfo->shapeDef;

  SliderXStyle * style = &guiInfo->styleInfo->sliderX;

  spriteCmp->forceDraw( style->backSpriteId, *shapeDef );
  Vec2Quad & r = shapeDef->rect;
  bool isVertical = false;
  float arrowAngle = PI * -0.5f;
  if( r.downLeft.y - r.upLeft.y > r.upRight.x - r.upLeft.x ) {
    //y > x czyli suwak pionowy
    isVertical = true;
    arrowAngle = 0.f;
  }

  ShapeDef shapeUpLeft, shapeDownRight;
  createRect( style->width, style->width, &shapeUpLeft.rect );
  translateQuad( shapeUpLeft.rect, Vec2( style->width, style->width ) * -0.5f ,
                 &shapeUpLeft.rect );

  shapeUpLeft.pos = shapeDef->pos + Vec2( style->width, style->width ) * 0.5f;
  shapeUpLeft.angle = arrowAngle;
  shapeUpLeft.depth = shapeDef->depth - 1;
  spriteCmp->forceDraw( style->upArrowSpriteId, shapeUpLeft );


  shapeDownRight = shapeUpLeft;
  shapeDownRight.pos = shapeDef->pos + shapeDef->rect.downRight -
                       Vec2( style->width, style->width ) * 0.5f;


  shapeDownRight.angle = arrowAngle + PI;
  shapeDownRight.depth = shapeDef->depth - 1;
  spriteCmp->forceDraw( style->upArrowSpriteId, shapeDownRight );
  //juz na rysowane, sprawdzam kliknięcie

  const GuiAction & action = guiCmp->getState();
  guiCmp->actionTest( e, shapeDef );
  if( guiCmp->isActive( e ) ) {
    Vec2 mousePos;
    int mx, my;
    SDL_GetMouseState( &mx, &my );
    mousePos = Vec2( mx, my );

    if( shapeUpLeft.isInside( mousePos ) )
    {
      return Widget::UP;
    }
    if( shapeDownRight.isInside( mousePos ) )
    {
      return Widget::DOWN;
    }
    if( action.buttonState & GuiCmp::WHEEL_UP ) {
      return Widget::UP;
    }
    if( action.buttonState & GuiCmp::WHEEL_DOWN ) {
      //puts( "SliderX::WHEEL_DOWN" );
      return Widget::DOWN;
    }

  }
  return 0;
}

static float calcNewSliderPosition_somoth( float boatY, float barLength )
{
  GuiAction & action = g_game->guiCmp->getState();
  const float len = action.mouseY - ( boatY )  ;
  float per2 = ( len ) / barLength;
  return -/*sign( per2 ) */ per2 ;
}

static float calcNewSliderPosition( float boatY,float boatH, float barLength )
{
  GuiAction & action = g_game->guiCmp->getState();
  const float len = boatY+ 0.5f*boatH - action.mouseY;
  return sign(len);
}

static float calcNewSliderPosition2( float boatY,float boatH, float barLength )
{
  GuiAction & action = g_game->guiCmp->getState();
  const float len = boatY+ 0.5f*boatH - action.mouseY;
  return len/boatH;
}

float sliderWertical( GuiInfo * guiInfo, float percent )
{
  assert( percent <= 1.f && percent >= 0.f );

  const Entity & e = guiInfo->entity;

  SpriteCmp * spriteCmp = g_game->getSpriteCmp();
  GuiCmp * guiCmp = g_game->getGuiCmp();
  ShapeDef * shapeDef = guiInfo->shapeDef;

  SliderXStyle * style = &guiInfo->styleInfo->sliderX;

  ShapeDef shapeUpLeft, shapeDownRight;
  createRect( style->width, style->width, &shapeUpLeft.rect );
  translateQuad( shapeUpLeft.rect, Vec2( style->width, style->width ) * -0.5f ,
                 &shapeUpLeft.rect );

  shapeUpLeft.pos = shapeDef->pos + Vec2( style->width, style->width ) * 0.5f;
  shapeUpLeft.angle = 0.f;
  shapeUpLeft.depth = shapeDef->depth - 1;
  spriteCmp->forceDraw( style->upArrowSpriteId, shapeUpLeft );


  shapeDownRight = shapeUpLeft;
  shapeDownRight.pos = shapeDef->pos + shapeDef->rect.downRight -
                       Vec2( style->width, style->width ) * 0.5f;


  shapeDownRight.angle = PI;
  shapeDownRight.depth = shapeDef->depth - 1;
  spriteCmp->forceDraw( style->upArrowSpriteId, shapeDownRight );
  //juz na rysowane, sprawdzam kliknięcie

  float out = 0;

  GuiInfo gi = *guiInfo;
  gi.shapeDef = &shapeUpLeft;
  switch( rectX( &gi, style->upArrowSpriteId ) ) {
    case Widget::LEFT_DOWN:
      out = 1;
      break;
    case Widget::WHEEL_UP:
    case Widget::WHEEL_DOWN:
      out = g_game->inputCmp->getWheelDelta();
      break;

  }

  gi.shapeDef = &shapeDownRight;
  switch( rectX( &gi, style->upArrowSpriteId ) ) {
    case Widget::LEFT_DOWN:
      out = -1;
      break;
    case Widget::WHEEL_UP:
    case Widget::WHEEL_DOWN:
      out = g_game->inputCmp->getWheelDelta();
      break;
  }

  ShapeDef backShape,boat;
  createRect( style->width - 2, style->width - 2, &boat.rect );
  boat.depth = guiInfo->shapeDef->depth - 1;
  boat.pos = shapeDef->pos +
             Vec2( 1.f, style->width +
                   (( shapeDef->rect.height() - 3 * style->width ) ) * percent );
  //koniec boata

  createRect( shapeDef->rect.width(), shapeDef->rect.height() - 2 * style->width,
              &backShape.rect );
  //
  backShape.pos = shapeDef->pos + Vec2( 0.f, style->width );
  backShape.depth = guiInfo->shapeDef->depth;
  gi.shapeDef = &backShape;
  switch( rectX( &gi, style->backSpriteId ) ) {
    case Widget::LEFT_DOWN:
      out = calcNewSliderPosition( boat.pos.y, boat.rect.height(),
                                   shapeDef->rect.height() - 2 * style->width );
      break;
    case Widget::WHEEL_UP:
    case Widget::WHEEL_DOWN:
      out = g_game->inputCmp->getWheelDelta();
      break;
  }

  gi.shapeDef = &boat;
  switch( rectX( &gi, style->barSpriteId ) ) {
    case Widget::LEFT_DOWN:
      out = calcNewSliderPosition2( boat.pos.y, boat.rect.height(),
                                   shapeDef->rect.height() - 2 * style->width );
    case Widget::LEFT_RELEASED:
      //guiCmp->unlockHot(e);
      break;
    case Widget::WHEEL_UP:
    case Widget::WHEEL_DOWN:
      out = g_game->inputCmp->getWheelDelta();
      break;
  }

  return out * 0.01f; //zamiana na procenty
}

uint32_t  windowX_2( const Entity e )
{
  GuiInfo * guiInfo;
  getRelated( e, &guiInfo );
  assert( guiInfo );
  return windowX_2( static_cast<GuiInfo *>( guiInfo ),0 );
}

uint32_t windowX_2( GuiInfo * guiInfo, WindowXScopedData * wxsd )
{
  GuiCmp * guiCmp = g_game->getGuiCmp();
  const Entity & e = guiInfo->entity;
  InputCmp * input = g_game->getInputCmp();

  WindowXStyle * style = &guiInfo->styleInfo->windowX;
  ShapeDef * const shapeDef = guiInfo->shapeDef;
  int16_t & depth = shapeDef->depth;

  int mouseX, mouseY;
  uint8_t mouseS = SDL_GetMouseState( &mouseX, &mouseY );
  Vec2 mousePos( mouseX, mouseY );
  Vec2 prevMousePos = Vec2( g_game->getInputCmp()->getPrevFrameMousePos() );

  const float & barHeight = style->barHeight;
  const float & borderWidth = style->borderWidth;

  const float width = shapeDef->rect.upRight.x - shapeDef->rect.upLeft.x;
  const float height = shapeDef->rect.downLeft.y - shapeDef->rect.upLeft.y;

  ShapeDef borderTop, borderBottom, borderLeft, borderRight;
  ShapeDef barShape;

  borderTop.depth = depth;
  borderBottom.depth = depth;
  borderLeft.depth = depth;
  borderRight.depth = depth;
  barShape.depth = depth;

  borderTop.pos = shapeDef->pos ;
  createRect( width , borderWidth, &borderTop.rect );

  createRect( width , borderWidth, &borderBottom.rect );
  borderBottom.pos = shapeDef->pos + Vec2( 0.f, height - borderWidth );

  createRect( borderWidth, height , &borderRight.rect );
  borderRight.pos = shapeDef->pos + Vec2( width - borderWidth, 0.f );

  borderLeft.pos = shapeDef->pos ;
  createRect( borderWidth, height , &borderLeft.rect );

  barShape.pos = shapeDef->pos  + Vec2( borderWidth, borderWidth );
  createRect( width - 2 * borderWidth, barHeight, &barShape.rect );


  WindowX & wx = guiInfo->windowX;
  const float & contentHeight = wx.contentSizeY;
  const float & contentWidth = wx.contentSizeX;

  int32_t gotScroll = 0;
  uint32_t toReturn = 0;

  GuiInfo gi = *guiInfo;

  if( contentHeight > shapeDef->rect.height() ) {
    //nalezy wyswietlić pionowy pasek przewijania
    //i go obsłużyć
    gotScroll |= 1;

    ShapeDef sliderShape; //definiuje obszar suwaka
    SliderXStyle * sliderStyle = &guiInfo->styleInfo->sliderX;

    float sliderHeight = shapeDef->rect.downRight.y - 2 * borderWidth - barHeight;
    if( contentWidth > shapeDef->rect.upRight.x )
    { //czyli jak bedzie wyświetlany także drugi pasek
      //zostawiam miejsce na poziomy pasek
      sliderHeight -= sliderStyle->width;
      gotScroll |= 2;
    }
    sliderShape.pos = shapeDef->pos + Vec2( shapeDef->rect.upRight.x -
                                            sliderStyle->width - borderWidth,
                                            borderWidth + barHeight );

    createRect( sliderStyle->width, sliderHeight,
                &sliderShape.rect );
    //troche płycej rysuje
    sliderShape.depth = depth - 1;

    gi.shapeDef = &sliderShape;
    //no i rysuje
    //slider przechwytuje bezpośrednio eventy okna i jest git :)
    float & a = wx.contentBeginY;
    float b = std::abs( wx.contentSizeY - height );
    a = clamp( a, 0.f, b );

    float d = sliderWertical( &gi, a / b ) ;
    a -= b * d;
  }
  if( contentWidth > shapeDef->rect.width() )
  { //poziomy pasek
    ShapeDef sliderShape;
    SliderXStyle * sliderStyle = &guiInfo->styleInfo->sliderX;

    sliderShape.depth = depth - 1;

    float sliderWidth = shapeDef->rect.downRight.x - 2 * borderWidth;
    if( contentHeight > shapeDef->rect.downLeft.y ) {
      sliderWidth -= sliderStyle->width;
    }
    sliderShape.pos = shapeDef->pos
                      + Vec2( borderWidth, shapeDef->rect.downLeft.y -
                              sliderStyle->width - borderWidth );
    createRect( sliderWidth, sliderStyle->width,
                &sliderShape.rect );

    gi.shapeDef = &sliderShape;

    float & a = wx.contentBeginX;
    float b = std::abs( wx.contentSizeX - width );  //100%

    a = clamp( a, 0.f, b );
//    float d = sliderHorizontal(&gi,a/b);
//    a -= b*d;
  }

  GuiAction & action = guiCmp->getState();
  //GuiAction & prev = guiCmp->getState();
  switch( guiCmp->actionTest( e, shapeDef ) ) //test całej powierzchni
  {
//    case Widget::HOVER:
//      g_game->shapeCmp->get( cursorId )->visible = false;
    default:;
  }


  //rysuje tło
  ShapeDef clientShape;
  calcWindowXUserRect( shapeDef, guiInfo->styleInfo, &clientShape, gotScroll );
  gi.shapeDef = &clientShape;

  rectX( &gi, style->backSpriteId ); //tło

  gi.shapeDef = &barShape;
  if( rectX( &gi, style->barSpriteId ) == Widget::LEFT_DOWN )
  {
    shapeDef->pos += Vec2( input->getMousePosDelta() );
    toReturn = Widget::MOVING;
  }

  gi.shapeDef = &borderTop;
  if( rectX( &gi, style->borderSpriteId ) == Widget::LEFT_DOWN )
  {
    Vec2 dy( 0.f, input->getMousePosDelta().y );
    shapeDef->pos += dy;
    shapeDef->rect.downLeft -= dy;
    shapeDef->rect.downRight -= dy;
    toReturn = Widget::RESIZING;
  }

  gi.shapeDef = &borderBottom;
  if( rectX( &gi, style->borderSpriteId ) == Widget::LEFT_DOWN )
  {

    Vec2 dy( 0.f, input->getMousePosDelta().y );
    shapeDef->rect.downLeft += dy;
    shapeDef->rect.downRight += dy;
    toReturn = Widget::RESIZING;
  }

  gi.shapeDef = &borderLeft;
  if( rectX( &gi, style->borderSpriteId ) == Widget::LEFT_DOWN )
  {
    Vec2 dx( input->getMousePosDelta().x, 0.f );
    shapeDef->pos += dx;
    shapeDef->rect.upRight -= dx;
    shapeDef->rect.downRight -= dx;
    toReturn = Widget::RESIZING;
  }

  gi.shapeDef = &borderRight;
  if( rectX( &gi, style->borderSpriteId ) == Widget::LEFT_DOWN )
  {
    Vec2 dx( input->getMousePosDelta().x, 0.f );
    shapeDef->rect.upRight += dx;
    shapeDef->rect.downRight += dx;
    toReturn = Widget::RESIZING;
  }

  if( width < style->borderWidth * 2 + 1 ) {
    shapeDef->rect.upRight.x = style->borderWidth * 2 + 1;
    shapeDef->rect.downRight.x = style->borderWidth * 2 + 1;
  }
  if( height < style->borderWidth * 2 + 1 + style->barHeight ) {
    shapeDef->rect.downLeft.y = style->borderWidth * 2 + 1 + style->barHeight;
    shapeDef->rect.downRight.y = style->borderWidth * 2 + 1 + style->barHeight;
  }

  if(wxsd)
  {
    g_game->getRender()->sortAndDrawSprites();
    ShapeDef& clientRect = wxsd->clientRect;
    clientRect.depth = guiInfo->shapeDef->depth - 1;
    calcWindowXUserRect( guiInfo->shapeDef,
                         guiInfo->styleInfo, &clientRect, gotScroll );

    ShapeDef & testRegion = wxsd->testRegion;
    guiCmp->getTestRegion( &testRegion );
    guiCmp->setTestRegion( clientRect );

    if( toReturn == Widget::MOVING ) {
      guiCmp->lockHot( e , Widget::MOVING );
    }
    clientRect.pos += Vec2( wx.contentBeginX, -wx.contentBeginY );

    wxsd->prevShape = guiInfo->shapeDef;
    guiInfo->shapeDef = &clientRect;

    wxsd->toReturn = toReturn;
    wxsd->guiInfo = guiInfo;
  }

  return toReturn;
}

void endWindowX(WindowXScopedData * wxsd)
{
  assert(wxsd);

  wxsd->guiInfo->shapeDef = wxsd->prevShape;

  g_game->guiCmp->unlockHot( wxsd->guiInfo->entity );
  g_game->guiCmp->setTestRegion( wxsd->testRegion );

  float winHeight = g_game->getRender()->getWindowDim().y;

  //włanczam obcinanie do okna
  glEnable( GL_SCISSOR_TEST );

  wxsd->clientRect.pos -= Vec2( wxsd->guiInfo->windowX.contentBeginX,
                          -wxsd->guiInfo->windowX.contentBeginY );

  int clHeight = wxsd->clientRect.rect.downRight.y -
    wxsd->clientRect.rect.upLeft.y;
  glScissor( wxsd->clientRect.pos.x ,
             winHeight - wxsd->clientRect.rect.downRight.y - wxsd->clientRect.pos.y ,
             wxsd->clientRect.rect.downRight.x,
             clHeight
           );
//
  //rysuje to co w oknie
  g_game->getRender()->sortAndDrawSprites();

  glDisable( GL_SCISSOR_TEST );
}


uint32_t guiTestToWigdet( GuiCmp * guiCmp, GuiCmp::TestValue t )
{
  GuiAction & action = guiCmp->getState();
  GuiAction & prev = guiCmp->getPrevState();

  switch( t )
  {
    case GuiCmp::HOT:
      return Widget::HOVER;
      break;
    case GuiCmp::ACTIVATED:
    case GuiCmp::ACTIVE:
      if( action.buttonState & GuiCmp::LEFT_DOWN )
        return Widget::LEFT_DOWN;
      else if( action.buttonState & GuiCmp::MIDDLE_DOWN )
        return Widget::MIDDLE_DOWN;
      else if( action.buttonState & GuiCmp::RIGHT_DOWN )
        return Widget::RIGHT_DOWN;
      else if( action.buttonState & GuiCmp::WHEEL_DOWN )
        return Widget::WHEEL_DOWN;
      else if( action.buttonState & GuiCmp::WHEEL_UP )
        return Widget::WHEEL_UP;
      break;
    case GuiCmp::RELEASED:
      if( prev.buttonState & GuiCmp::LEFT_DOWN )
        return Widget::LEFT_RELEASED;
      else if( prev.buttonState & GuiCmp::MIDDLE_DOWN )
        return Widget::MIDDLE_RELEASED;
      else if( prev.buttonState & GuiCmp::RIGHT_DOWN )
        return Widget::RIGHT_RELEASED;
      else if( prev.buttonState & GuiCmp::WHEEL_DOWN )
        return Widget::WHEEL_DOWN_RELEASED;
      else if( prev.buttonState & GuiCmp::WHEEL_UP )
        return Widget::WHEEL_UP_RELEASED;
      break;
  }
  return 0;
}

uint32_t rectX( const Entity e, Entity backId )
{
  return rectX( g_game->guiCmp->getOrAdd( e ), backId );
}
uint32_t rectX( GuiInfo * guiInfo, Entity backId )
{
  assert( guiInfo );
  ShapeDef & shape = *guiInfo->shapeDef;

  g_game->spriteCmp->forceDraw( backId, shape );

  return guiTestToWigdet( g_game->guiCmp,
                          g_game->guiCmp->actionTest( guiInfo->entity,
                              &shape ) );
}

uint32_t spriteX( const Entity e )
{
  return spriteX( g_game->guiCmp->getOrAdd( e ) );
}
uint32_t spriteX( GuiInfo * guiInfo )
{
  SpriteX & sx = guiInfo->spriteX;
  ShapeDef & d = *guiInfo->shapeDef;
  ShapeDef shape;
  translateQuad( d.rect, d.pos, &shape.rect );
  rotateQuad( shape.rect, d.angle, d.pos, &shape.rect );

  g_game->getRender()->drawAtlas( sx.atlasInfo->tex, sx.spriteId, shape.rect,
                                  Render::SCREEN_COORD,
                                  makeARGB( 255, 255, 255, 255 ), d.depth );
  //
  return guiTestToWigdet( g_game->guiCmp,
                          g_game->guiCmp->actionTest( guiInfo->entity, &d ) );

}
uint32_t drawAtlasCells( GuiInfo * windowGui, GuiData * guiData )
{
  const AtlasInfo * ai = g_game->getRender()->getAtlas( "img/by_ftorek.png" );
  const int32_t numSprites = ai->textureInfo.size();
  GuiInfo gi2 = *windowGui;
  //gi2.entity = Entity(-4);
  gi2.spriteX.atlasInfo = ai;
  ShapeDef * prevShape = windowGui->shapeDef;
  ShapeDef cellShape;
  gi2.shapeDef = &cellShape;
  gi2.shapeDef->depth = windowGui->shapeDef->depth;


  int32_t cell = 0;
  const int TILE_SIZE = 32;
  const int32_t WIDTH = prevShape->rect.width() / float( TILE_SIZE );
  for( int y = 0; y < numSprites; ++y ) {
    for( int x = 0; x < ( int )WIDTH; ++x, ++cell ) {
      if( cell >= numSprites ) {
        y = numSprites;
        x = WIDTH;
        break;
      }

      createRect( TILE_SIZE, TILE_SIZE, &cellShape.rect );
      cellShape.pos = prevShape->pos + Vec2(( x % WIDTH ) * TILE_SIZE,
                                            y * TILE_SIZE );

      gi2.spriteX.spriteId = cell;

      switch( spriteX( &gi2 ) )
      {
        case Widget::LEFT_DOWN:
          guiData->leftMouseCell = cell;
          SpriteInfo * si = g_game->spriteCmp->getOrAdd( cursorId );
          si->tex = ai->tex;
          si->textureNumber = cell;
          si->color = 0xffffffff;
          si->visible = true;

          break;
      }
    }/*koniec for (x)*/
  }/*koniec for (y)*/
  gi2.shapeDef = prevShape;
  windowGui->windowX.contentSizeX = WIDTH * TILE_SIZE;
  windowGui->windowX.contentSizeY = TILE_SIZE + TILE_SIZE * ( numSprites ) / ( float )WIDTH;
  return 0;
}

void drawGui( Game * game, GuiData * guiData )
{
  assert( game );
  assert( g_game );
  GuiCmp * guiCmp = game->getGuiCmp();

  static const Entity someWindow  = game->createGivenEntity( Entity( -2 ) );
  static const Entity innerRect = game->createGivenEntity( Entity( -3 ) );
  //static const


  GuiInfo * guiInfo = game->guiCmp->getOrAdd( someWindow );
  WindowXScopedData scoped;
  windowX_2( guiInfo, &scoped );
  drawAtlasCells(guiInfo,guiData);
  endWindowX(&scoped);



}
