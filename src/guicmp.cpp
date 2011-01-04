#include "guicmp.h"
#include "main.h"
#include "motioncmp.h"
#include "parser.h"
#include "widget.h"

const Entity GuiCmp::notFocused;


void consoleX( const Entity e, Game * game ,
               WidgetAction * a, ShapeDef * shapeDef )
{
  /*
    ShapeCmp * shapeCmp = game->getShapeCmp();
    GuiCmp * guiCmp = game->getGuiCmp();

    WidgetAction gameAction = guiCmp->actionTest( gameEntity );
    *a = guiCmp->actionTest( e );
    ShapeDef* &sh = a->shapeDef;
    const XY<int32_t> winDim = XY<int32_t>( game->getRender()->getWindowDim() );
    //const int maxHeight = winDim.y * 0.5f;
    const int maxHeight = 80;

    a->textInfo = game->getTextCmp()->get( e );
    TextInfo qwahaey3__4334;
    if(!a->textInfo){
      a->textInfo = &qwahaey3__4334;
    }
    if(! a->shapeDef ){
      ShapeDef sd;
      sd.pos = Vec2(89.f,89.f);
      sd.rect.upRight = Vec2(0.f,100.f);
      sd.rect.downLeft =Vec2(100.f,0.f);
      sd.rect.downRight=Vec2(100.f,100.f);
      sd.visible = true;
      a->shapeDef = shapeCmp->add(e,sd);
    }

    if( !a->shapeDef->visible )
    { // konsolka niewidoczna
      a->textInfo->visible = false;
      if( gameAction.key == SDLK_BACKQUOTE )  //pokazuje kontrolke
      { //wcisnieto ` i nalezy ja pokazac
        //guiCmp->makeFocused(e);
        //guiCmp->focusNext( &a );

        sh->pos = Vec2( 0.f, 0.f );
        sh->rect.upLeft = Vec2( 0.f, 0.f );
        sh->rect.upRight = Vec2( winDim.x, 0.f );

        sh->rect.downLeft = Vec2( 0.f, 0.f );
        sh->rect.downRight = Vec2( winDim.x, 0.f );


        a->textInfo->visible = true;
        //a->textInfo->position = Vec2(10.f,10.f);
        //a->textInfo->color = makeARGB(255,200,10,10);
        sh->visible = true;    //rozwijaj
      }
      //toto niedziała narazie
      if( sh->rect.downLeft.y > 0 )  //zwijam panie mieciu :P
      {
        sh->rect.downLeft.y -= 1;
        sh->rect.downRight.y -= 1;
      }
    }
    else { //widzialna
      //a->textInfo = game->getTextCmp()->get(e);
      //a->textInfo->visible = true;
      //a->textInfo->position = Vec2(10.f,10.f);
      //a->textInfo->color = makeARGB(255,200,10,10);

      if( gameAction.key == SDLK_BACKQUOTE || a->key == SDLK_BACKQUOTE )
      { //pora sie schowac
        sh->visible = false;
        a->textInfo->visible = false;
        //oddaje focus
        guiCmp->makeFocused( gameEntity );
      }
      //rozwijanie
      if( sh->rect.downLeft.y < maxHeight )
      {
        sh->rect.downLeft.y += 1;
        sh->rect.downRight.y += 1;
      }
    }
  */
  return ;
}

//drugi paramettr to tablica 4 wskażników
void makeBorder( const ShapeDef & q, const ShapeDef * out[4] )
{

}

void  windowX( const Entity e, Game * game,
               WidgetAction * action, GuiInfo * guiInfo )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();
  //GuiInfo * guiInfo = game->getGuiCmp()->getSure( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();

  //musi sie powieść :P
  WindowXStyle * style =
    &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->windowX;

  ShapeDef * shapeDef = guiInfo->shapeDef;

  if( ! shapeDef )
    return;

  int16_t & depth = shapeDef->depth;
  if( guiCmp->isActive( e ) || guiCmp->isLocked( e ) ) {
  }
  //printf("e '%u' depth: %hi\n",e.getId(),depth);

  spriteCmp->draw( style->backSpriteId, *shapeDef );

  int mouseX, mouseY;
  uint8_t mouseS = SDL_GetMouseState( &mouseX, &mouseY );
  Vec2 mousePos( mouseX, mouseY );

  ShapeDef barShape;
  const float & barHeight = style->barHeight;
  const float & borderWidth = style->borderWidth;

  barShape.pos = shapeDef->pos - Vec2( 0.f, barHeight );
  barShape.rect.upRight = Vec2( shapeDef->rect.upRight.x, 0.f );
  barShape.rect.downLeft = Vec2( 0.f, barHeight );
  barShape.rect.downRight = Vec2( shapeDef->rect.upRight.x, barHeight );
  barShape.visible = true;
  barShape.depth = depth;

  //rysowanie
  game->getSpriteCmp()->forceDraw( style->barSpriteId, barShape );


  ShapeDef borderTop, borderBottom, borderLeft, borderRight;
  borderTop.depth = depth;
  borderBottom.depth = depth;
  borderLeft.depth = depth;
  borderRight.depth = depth;

  borderTop.pos = barShape.pos - Vec2( borderWidth, borderWidth );
  createRect( shapeDef->rect.upRight.x + 2 * borderWidth, borderWidth,
              &borderTop.rect );


  createRect( shapeDef->rect.downRight.x + 2 * borderWidth, borderWidth,
              &borderBottom.rect );
  borderBottom.pos = shapeDef->pos + shapeDef->rect.downLeft
                     - Vec2( borderWidth, 0.f );


  createRect( borderWidth, shapeDef->rect.downRight.y + 2 * borderWidth + barHeight,
              &borderRight.rect );
  borderRight.pos = shapeDef->pos + shapeDef->rect.upRight
                    - Vec2( 0.f, barHeight + borderWidth );

  borderLeft.pos = barShape.pos - Vec2( borderWidth, borderWidth );
  createRect( borderWidth, shapeDef->rect.downLeft.y + 2 * borderWidth + barHeight,
              &borderLeft.rect );
  //cały czas rysowanie

  spriteCmp->forceDraw( style->borderSpriteId, borderTop );
  spriteCmp->forceDraw( style->borderSpriteId, borderBottom );
  spriteCmp->forceDraw( style->borderSpriteId, borderRight );
  spriteCmp->forceDraw( style->borderSpriteId, borderLeft );

  InputCmp * input = game->getInputCmp();

  GuiAction barAction = guiCmp->actionTest( e, &barShape );

  GuiAction borderTopAction = guiCmp->actionTest( e, &borderTop );
  GuiAction borderBottomAction = guiCmp->actionTest( e, &borderBottom );
  GuiAction borderLeftAction = guiCmp->actionTest( e, &borderLeft );
  GuiAction borderRightAction = guiCmp->actionTest( e, &borderRight );

  if( !guiCmp->isLocked( e ) )
  {
    if( barAction.hotInfo & GuiCmp::LEFT_DOWN )
    { //czyli jak wciśnięto lewy, i wcześniej nie było żadnej akcji

      guiInfo->state |= GuiInfo::MOVING;
      action->u32 = WidgetCommon::MOVE;
      guiCmp->lockHot( e, GuiInfo::MOVING );

    }
    else if( borderTopAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_TOP;
      action->u32 = WidgetCommon::RESIZE_TOP;
      guiCmp->lockHot( e , GuiInfo::RESIZING_TOP );
    }
    else if( borderBottomAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_BOTTOM;
      action->u32 = WidgetCommon::RESIZE_BOTTOM;
      guiCmp->lockHot( e , GuiInfo::RESIZING_BOTTOM );
    }
    else if( borderLeftAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_LEFT;
      action->u32 = WidgetCommon::RESIZE_LEFT;
      guiCmp->lockHot( e, GuiInfo::RESIZING_LEFT );
    }
    else if( borderRightAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_RIGHT;
      action->u32 = WidgetCommon::RESIZE_RIGHT;
      guiCmp->lockHot( e , GuiInfo::RESIZING_RIGHT );
    }
  }
  else { /*( guiCmp->isLocked( e ) )*/
    /* wujowo `troche` to opracowałem.
      Prościej: można było zrobić prościej: wyznaczyć obrys CAŁEJ kontrolki:
      z belką, obramowaniem i sprawdzić actionTestem wtedy zamiast 5 sprawdzeń
      było by jedno i było by łatwiej
    */
    uint32_t reason = guiCmp->getLockReason();
    switch( reason )
    {
      case GuiInfo::MOVING:
        if( !( barAction.hotInfo & GuiCmp::LEFT_DOWN ) ) {
          guiCmp->unlockHot( e );
        }
        else {
          shapeDef->pos += Vec2( game->getInputCmp()->getMousePosDelta() );
        }
        break;
      case GuiInfo::RESIZING_TOP:

        if( !( borderTopAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dy( 0.f, input->getMousePosDelta().y );
          shapeDef->pos += dy;
          shapeDef->rect.downLeft -= dy;
          shapeDef->rect.downRight -= dy;
        }

        break;
      case GuiInfo::RESIZING_BOTTOM:
      {
        if( !( borderBottomAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dy( 0.f, input->getMousePosDelta().y );
          shapeDef->rect.downLeft += dy;
          shapeDef->rect.downRight += dy;
        }
      }
      break;
      case GuiInfo::RESIZING_LEFT:
        if( !( borderLeftAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dx( input->getMousePosDelta().x, 0.f );
          shapeDef->pos += dx;
          shapeDef->rect.upRight -= dx;
          shapeDef->rect.downRight -= dx;
        }
        break;
      case GuiInfo::RESIZING_RIGHT:
        if( !( borderRightAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dx( input->getMousePosDelta().x, 0.f );
          shapeDef->rect.upRight += dx;
          shapeDef->rect.downRight += dx;
        }
        break;
    }
  }

//  glEnable(GL_SCISSOR_TEST);
//  glScissor(50,50,300,400);

  WindowX & wx = guiInfo->widgetData.windowX;
  const float & contentHeight = wx.contentSizeY;
  const float & contentWidth = wx.contentSizeX;

  if( contentHeight > shapeDef->rect.downLeft.y ) {
    //nalezy wyswietlić pionowy pasek przewijania
    //i go obsłużyć

    ShapeDef sliderShape; //definiuje obszar suwaka
    SliderXStyle * sliderStyle = //do tego potrzeby jest styl
      &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->sliderX;

    float sliderHeight = shapeDef->rect.downRight.y;
    if( contentWidth > shapeDef->rect.upRight.x )
    { //czyli jak bedzie wyświetlany także drugi pasek
      //zostawiam miejsce na poziomy pasek
      sliderHeight -= sliderStyle->width;
    }
    sliderShape.pos = shapeDef->pos
                      + Vec2( shapeDef->rect.upRight.x - sliderStyle->width, 0.f );
    createRect( sliderStyle->width, sliderHeight,
                &sliderShape.rect );
    //troche płycej rysuje
    sliderShape.depth = depth - 1;

    //no i rysuje
    //slider przechwytuje bezpośrednio eventy okna i jest git :)
    sliderX( e, game, action, &sliderShape );
    switch( action->msg )
    {
      case SliderX::WHEEL_DOWN:
      case SliderX::DOWN_LEFT_CLICKED:
        wx.scrollVer++;
        break;
      case SliderX::WHEEL_UP:
      case SliderX::UP_RIGHT_CLICKED:
        wx.scrollVer--;
        break;
      default:
        break;
    }

  }
  if( contentWidth > shapeDef->rect.upRight.x )
  {
    ShapeDef sliderShape;
    SliderXStyle * sliderStyle =
      &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->sliderX;

    sliderShape.depth = depth - 1;

    float sliderWidth = shapeDef->rect.downRight.x;
    if( contentHeight > shapeDef->rect.downLeft.y ) {
      sliderWidth -= sliderStyle->width;
    }
    sliderShape.pos = shapeDef->pos
                      + Vec2( 0.f, shapeDef->rect.downLeft.y - sliderStyle->width );
    createRect( sliderWidth, sliderStyle->width,
                &sliderShape.rect );

    sliderX( e, game, action, &sliderShape );
    switch( action->msg )
    {
      case SliderX::WHEEL_DOWN:
      case SliderX::DOWN_LEFT_CLICKED:
        wx.scrollHor++;
        break;
      case SliderX::WHEEL_UP:
      case SliderX::UP_RIGHT_CLICKED:
        wx.scrollHor--;
        break;
      default:
        break;
    }
  }

  return ;

}

/*  scroll - czy są scroll bary?
    0 - brak
    1 - pionowy
    2 - poziomy
    3 - pionowy i poziomy
*/
void calcWindowXUserRect(ShapeDef * shapeDef,GuiStyleInfo * gsi,
                         ShapeDef* out,int gotScroll)
{
  WindowXStyle * ws = &gsi->windowX;
  out->pos = shapeDef->pos + Vec2(ws->borderWidth,
                                  ws->borderWidth+ws->barHeight);
  float scrlVerS = 0.f;
  float scrlHorS = 0.f;
  if( gotScroll & 1){
    scrlVerS = gsi->sliderX.width;
  }
  if( gotScroll & 2){
    scrlHorS = gsi->sliderX.width;
  }

  createRect(shapeDef->rect.upRight.x - shapeDef->rect.upLeft.x -
             2*ws->borderWidth - scrlVerS,
             shapeDef->rect.downLeft.y - shapeDef->rect.upLeft.y -
             2*ws->borderWidth - ws->barHeight - scrlHorS,&out->rect );
}

/*  poprzednia implementacja okna
    uznaawała podane wymairy jako wymiary obszaru klienta, ta bedzie je
    traktowac jak wymiary całego okna
*/
void  windowX_2( const Entity e, Game * game,
               WidgetAction * action, GuiInfo * guiInfo )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();
  SpriteCmp * spriteCmp = game->getSpriteCmp();

  //musi sie powieść :P
  WindowXStyle * style = &guiInfo->styleInfo->windowX;

  ShapeDef * shapeDef = guiInfo->shapeDef;

  if( ! shapeDef )
    return;

  int16_t & depth = shapeDef->depth;
  if( guiCmp->isActive( e ) || guiCmp->isLocked( e ) ) {
  }
  //printf("e '%u' depth: %hi\n",e.getId(),depth);

  int mouseX, mouseY;
  uint8_t mouseS = SDL_GetMouseState( &mouseX, &mouseY );
  Vec2 mousePos( mouseX, mouseY );

  const float & barHeight = style->barHeight;
  const float & borderWidth = style->borderWidth;

  ShapeDef borderTop, borderBottom, borderLeft, borderRight;
  borderTop.depth = depth;
  borderBottom.depth = depth;
  borderLeft.depth = depth;
  borderRight.depth = depth;

  borderTop.pos = shapeDef->pos ;
  createRect( shapeDef->rect.upRight.x , borderWidth,
              &borderTop.rect );

  createRect( shapeDef->rect.downRight.x , borderWidth,
              &borderBottom.rect );
  borderBottom.pos = shapeDef->pos + shapeDef->rect.downLeft
                     - Vec2( 0.f, borderWidth );


  createRect( borderWidth, shapeDef->rect.downRight.y ,
              &borderRight.rect );
  borderRight.pos = shapeDef->pos + shapeDef->rect.upRight
                    - Vec2( borderWidth, 0.f );

  borderLeft.pos = shapeDef->pos ;
  createRect( borderWidth, shapeDef->rect.downLeft.y ,
              &borderLeft.rect );
  //cały czas rysowanie

  spriteCmp->forceDraw( style->borderSpriteId, borderTop );
  spriteCmp->forceDraw( style->borderSpriteId, borderBottom );
  spriteCmp->forceDraw( style->borderSpriteId, borderRight );
  spriteCmp->forceDraw( style->borderSpriteId, borderLeft );

  ShapeDef barShape;

  barShape.pos = shapeDef->pos  + Vec2(borderWidth,borderWidth);
  barShape.rect.upRight = Vec2( shapeDef->rect.upRight.x - 2*borderWidth, 0.f );
  barShape.rect.downLeft = Vec2( 0.f, barHeight );
  barShape.rect.downRight = Vec2( shapeDef->rect.upRight.x- 2*borderWidth, barHeight );
  barShape.visible = true;
  barShape.depth = depth;

  //rysowanie
  game->getSpriteCmp()->forceDraw( style->barSpriteId, barShape );

  WindowX & wx = guiInfo->widgetData.windowX;
  const float & contentHeight = wx.contentSizeY;
  const float & contentWidth = wx.contentSizeX;

  int32_t gotScroll = 0;

  if( contentHeight > shapeDef->rect.downLeft.y ) {
    //nalezy wyswietlić pionowy pasek przewijania
    //i go obsłużyć
    gotScroll |= 1;

    ShapeDef sliderShape; //definiuje obszar suwaka
    SliderXStyle * sliderStyle = //do tego potrzeby jest styl
      &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->sliderX;

    float sliderHeight = shapeDef->rect.downRight.y - 2*borderWidth - barHeight;
    if( contentWidth > shapeDef->rect.upRight.x )
    { //czyli jak bedzie wyświetlany także drugi pasek
      //zostawiam miejsce na poziomy pasek
      sliderHeight -= sliderStyle->width;
      gotScroll |= 2;
    }
    sliderShape.pos =
      shapeDef->pos + Vec2( shapeDef->rect.upRight.x -
                           sliderStyle->width - borderWidth,
                           borderWidth + barHeight );

    createRect( sliderStyle->width, sliderHeight,
                &sliderShape.rect );
    //troche płycej rysuje
    sliderShape.depth = depth - 1;

    //no i rysuje
    //slider przechwytuje bezpośrednio eventy okna i jest git :)
    sliderX( e, game, action, &sliderShape );
    switch( action->msg )
    {
      case SliderX::WHEEL_DOWN:
      case SliderX::DOWN_LEFT_CLICKED:
        wx.scrollVer++;
        break;
      case SliderX::WHEEL_UP:
      case SliderX::UP_RIGHT_CLICKED:
        wx.scrollVer--;
        break;
      default:
        break;
    }

  }
  if( contentWidth > shapeDef->rect.upRight.x )
  {
    ShapeDef sliderShape;
    SliderXStyle * sliderStyle =
      &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->sliderX;

    sliderShape.depth = depth - 1;

    float sliderWidth = shapeDef->rect.downRight.x - 2*borderWidth;
    if( contentHeight > shapeDef->rect.downLeft.y ) {
      sliderWidth -= sliderStyle->width;
    }
    sliderShape.pos = shapeDef->pos
                      + Vec2( borderWidth, shapeDef->rect.downLeft.y -
                             sliderStyle->width - borderWidth);
    createRect( sliderWidth, sliderStyle->width,
                &sliderShape.rect );

    sliderX( e, game, action, &sliderShape );
    switch( action->msg )
    {
      case SliderX::WHEEL_DOWN:
      case SliderX::DOWN_LEFT_CLICKED:
        wx.scrollHor++;
        break;
      case SliderX::WHEEL_UP:
      case SliderX::UP_RIGHT_CLICKED:
        wx.scrollHor--;
        break;
      default:
        break;
    }
  }
  //rysuje tło
  ShapeDef clientShape;
  calcWindowXUserRect(shapeDef,guiInfo->styleInfo,&clientShape,gotScroll);
  spriteCmp->draw( style->backSpriteId, clientShape );


  InputCmp * input = game->getInputCmp();

  GuiAction barAction = guiCmp->actionTest( e, &barShape );

  GuiAction borderTopAction = guiCmp->actionTest( e, &borderTop );
  GuiAction borderBottomAction = guiCmp->actionTest( e, &borderBottom );
  GuiAction borderLeftAction = guiCmp->actionTest( e, &borderLeft );
  GuiAction borderRightAction = guiCmp->actionTest( e, &borderRight );

  if( !guiCmp->isLocked( e ) )
  {
    if( barAction.hotInfo & GuiCmp::LEFT_DOWN )
    { //czyli jak wciśnięto lewy, i wcześniej nie było żadnej akcji

      guiInfo->state |= GuiInfo::MOVING;
      action->u32 = WidgetCommon::MOVE;
      guiCmp->lockHot( e, GuiInfo::MOVING );

    }
    else if( borderTopAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_TOP;
      action->u32 = WidgetCommon::RESIZE_TOP;
      guiCmp->lockHot( e , GuiInfo::RESIZING_TOP );
    }
    else if( borderBottomAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_BOTTOM;
      action->u32 = WidgetCommon::RESIZE_BOTTOM;
      guiCmp->lockHot( e , GuiInfo::RESIZING_BOTTOM );
    }
    else if( borderLeftAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_LEFT;
      action->u32 = WidgetCommon::RESIZE_LEFT;
      guiCmp->lockHot( e, GuiInfo::RESIZING_LEFT );
    }
    else if( borderRightAction.hotInfo & GuiCmp::LEFT_DOWN )
    {
      guiInfo->state |= GuiInfo::RESIZING_RIGHT;
      action->u32 = WidgetCommon::RESIZE_RIGHT;
      guiCmp->lockHot( e , GuiInfo::RESIZING_RIGHT );
    }
  }
  else { /*( guiCmp->isLocked( e ) )*/
    /* wujowo `troche` to opracowałem.
      Prościej: można było zrobić prościej: wyznaczyć obrys CAŁEJ kontrolki:
      z belką, obramowaniem i sprawdzić actionTestem wtedy zamiast 5 sprawdzeń
      było by jedno i było by łatwiej
    */
    uint32_t reason = guiCmp->getLockReason();
    switch( reason )
    {
      case GuiInfo::MOVING:
        if( !( barAction.hotInfo & GuiCmp::LEFT_DOWN ) ) {
          guiCmp->unlockHot( e );
        }
        else {
          shapeDef->pos += Vec2( game->getInputCmp()->getMousePosDelta() );
        }
        break;
      case GuiInfo::RESIZING_TOP:

        if( !( borderTopAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dy( 0.f, input->getMousePosDelta().y );
          shapeDef->pos += dy;
          shapeDef->rect.downLeft -= dy;
          shapeDef->rect.downRight -= dy;
        }

        break;
      case GuiInfo::RESIZING_BOTTOM:
      {
        if( !( borderBottomAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dy( 0.f, input->getMousePosDelta().y );
          shapeDef->rect.downLeft += dy;
          shapeDef->rect.downRight += dy;
        }
      }
      break;
      case GuiInfo::RESIZING_LEFT:
        if( !( borderLeftAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dx( input->getMousePosDelta().x, 0.f );
          shapeDef->pos += dx;
          shapeDef->rect.upRight -= dx;
          shapeDef->rect.downRight -= dx;
        }
        break;
      case GuiInfo::RESIZING_RIGHT:
        if( !( borderRightAction.hotInfo & GuiCmp::LEFT_DOWN ) )
          guiCmp->unlockHot( e );
        else {
          Vec2 dx( input->getMousePosDelta().x, 0.f );
          shapeDef->rect.upRight += dx;
          shapeDef->rect.downRight += dx;
        }
        break;
    }
  }

  return ;

}

void sliderX( const Entity e, Game * game,
              WidgetAction * action, ShapeDef * shapeDef )
{
  SpriteCmp * spriteCmp = game->getSpriteCmp();

  GuiInfo * guiInfo = game->getGuiCmp()->getSure( e );

  SliderXStyle * style =
    &game->getGuiStyleCmp()->getSure( guiInfo->styleId )->sliderX;

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
  shapeUpLeft.depth = shapeDef->depth - 1;
  spriteCmp->forceDraw( style->upArrowSpriteId, shapeDownRight );
  //juz na rysowane, sprawdzam kliknięcie

  if( action->hotInfo & GuiCmp::LEFT_RELEASED ) {
    Vec2 mousePos;
    int mx, my;
    SDL_GetMouseState( &mx, &my );
    mousePos = Vec2( mx, my );

    if( shapeUpLeft.isInside( mousePos ) )
    {
      action->msg = SliderX::UP_RIGHT_CLICKED;
      puts( "SliderX::UP_RIGHT_CLICKED" );

    }
    else if( shapeDownRight.isInside( mousePos ) )
    {
      action->msg = SliderX::DOWN_LEFT_CLICKED;
    }
  }
  else if( action->hotInfo & GuiCmp::WHEEL_UP )
  {
    action->msg = SliderX::WHEEL_UP;
  }
  else if( action->hotInfo & GuiCmp::WHEEL_DOWN )
  {
    action->msg = SliderX::WHEEL_DOWN;
    puts( "SliderX::WHEEL_DOWN" );
  }

}

void spriteX( const Entity e, Game * game, WidgetAction * inout,
              GuiInfo * guiInfo )
{
  SpriteX & sx = guiInfo->widgetData.spriteX;
  ShapeDef & d = *guiInfo->shapeDef;
  Vec2Quad shape;
  translateQuad( d.rect, d.pos, &shape );
  rotateQuad( shape, d.angle, d.pos, &shape );

  game->getRender()->drawAtlas( sx.atlasInfo->tex, sx.spriteId, shape,
                                Render::SCREEN_COORD,
                                makeARGB( 255, 255, 255, 255 ), d.depth );

}

static void eraseMarkedTekst( std::string & str, int16_t & cursor, int16_t & mark )
{
  int16_t mark2 = mark;
  int16_t cursor2 = cursor;
  if( mark < 0 ) {
    mark2 = -mark2;
    cursor2 = cursor2 + mark;
    cursor += mark;
  }
  str.erase( cursor2, mark2 );

  mark = 0;
}

void editX( const Entity e , Game * game,
            WidgetAction * action, ShapeDef * shapeDef )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();
  SpriteCmp * spriteCmp = game->getSpriteCmp();

  TextInfo * textInfo = textCmp->get( e );
  if( !textInfo )
    return;
  //a moze jak text info ==0 to return ??

  if( !shapeDef )
    return ;

  //  shapeDef->visible = true;
  //  spriteCmp->draw( e );
  //  shapeDef->visible = false;

  std::string & str = textInfo->text;

  textInfo->position = shapeDef->pos;
  textInfo->visible = true;
  textCmp->draw( e );
  textInfo->visible = false;

  if( action->hotInfo & GuiCmp::FOCUSED ) {

    int16_t & cursor = guiCmp->cursor;
    int16_t & mark = guiCmp->mark;

    cursor = std::min( int16_t( str.size() ), cursor );

    switch( action->key )
    {
      case SDLK_BACKSPACE:
        if( mark != 0 ) {
          eraseMarkedTekst( str, cursor, mark );
        }
        else if( guiCmp->cursor > 0 ) {
          str.erase( guiCmp->cursor - 1, 1 );
          --guiCmp->cursor;
        }
        break;
      case SDLK_DELETE:
        if( mark != 0 ) {
          eraseMarkedTekst( str, cursor, mark );
        }
        if( guiCmp->cursor < str.size() ) {
          str.erase( guiCmp->cursor, 1 );
        }
        break;

      case SDLK_LEFT:
        cursor = std::max( int16_t( 0 ), --guiCmp->cursor );
        if( action->keyMod & KMOD_CTRL ) {
          uint32_t found =
            str.find_last_of( ",./;\'[]\\=-~!@#$%^&*()_+ <>?:\"{}|", cursor - 1 );
          //str.find_last_not_of( "qwertyuiopasdfghjklzxcvbnm", cursor );

          if( found != std::string::npos )
          { //nie znaleziono
            mark = cursor + 1;
            cursor = found;
          }
          else {
            mark = cursor + 1;
            cursor = 0;
          }
          //nie trzeba zaznaczać, bo brak shifta
          if( !( action->keyMod & KMOD_SHIFT ) ) {
            mark = 0;
          }

        }
        else if( action->keyMod & KMOD_SHIFT ) {
          ++mark;
        }
        else {

          mark = 0;
        }
        break;
      case SDLK_RIGHT:
        cursor = std::min( int16_t( str.size() ), ++guiCmp->cursor );
        if( action->keyMod & KMOD_SHIFT )
        {
          //kursor oddala sie od zaznaczenia
          --mark;
        }
        else {

          mark = 0;
        }
        break;
      case SDLK_HOME:
        guiCmp->cursor = 0;
        break;
      case SDLK_END:
        guiCmp->cursor = str.size();
        break;

      case 0:
      case SDLK_LCTRL:
      case SDLK_RCTRL:
      case SDLK_LALT:
      case SDLK_RALT:
      case SDLK_RETURN:
      case SDLK_UP:
      case SDLK_DOWN:
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:

        break;
      default:
        if( mark != 0 ) {
          eraseMarkedTekst( str, cursor, mark );
        }
        str.insert( guiCmp->cursor++, 1, action->unicode );

        //str += action->key;
        break;
    }//switch(action->key)

    //rysowanie kursora
    int width = textCmp->getTextWidth( str.c_str(), textInfo->font, cursor );
    int height = textCmp->getFontHeight( textInfo->font );
    const int cursorWidth = 2;

    ShapeDef def;
    def.pos = textInfo->position + Vec2( width, 0.f );
    def.visible = true;
    def.rect.upLeft = RenderVec2( 0.f, 0.f );
    def.rect.upRight = RenderVec2( cursorWidth, 0.f );
    def.rect.downLeft = RenderVec2( 0.f, height );
    def.rect.downRight = RenderVec2( cursorWidth, height );
    def.depth = textInfo->depth;

    spriteCmp->drawRect( def, textInfo->color );

    if( mark != 0 )
    { //cos zaznaczono
      int mark2 = cursor + mark;
      int cursor2 = cursor;
      if( mark < 0 ) { //zamiana pozycji
        mark2 = cursor;
        cursor2 = cursor + mark;
      }

      int markEndWidth = textCmp->getTextWidth( str.c_str(), textInfo->font,
                         mark2 );
      int markBeginWidth = textCmp->getTextWidth( str.c_str(), textInfo->font,
                           cursor2 );

      def.pos = textInfo->position + Vec2( markBeginWidth, 0.f );
      const int markWidth = markEndWidth - markBeginWidth;
      def.visible = true;
      def.rect.upLeft = RenderVec2( 0.f, 0.f );
      def.rect.upRight = RenderVec2( markWidth, 0.f );
      def.rect.downLeft = RenderVec2( 0.f, height );
      def.rect.downRight = RenderVec2( markWidth, height );
      def.depth = textInfo->depth;

      spriteCmp->drawRect( def, makeARGB( 150, 220, 30, 30 ) );

    }

  }

  return;
}

void drawMapTiles( const Entity e, Game * game, WidgetAction * inout,
                   GuiInfo * guiInfo )
{
  const AtlasInfo * ai = game->getRender()->getAtlas( "img/by_ftorek.png" );
  if( !ai )
    return;
  GuiCmp * guiCmp = game->getGuiCmp();

  windowX_2( e, game, inout, guiInfo );

  const int32_t Y = ai->textureInfo.size();
  ShapeDef shapeDef = *guiInfo->shapeDef;   //copy plx
  ShapeDef clientRect = *guiInfo->shapeDef;
  calcWindowXUserRect(&shapeDef,guiInfo->styleInfo,&clientRect,3);

  float winHeight = game->getRender()->getWindowDim().y;
  game->getRender()->sortAndDrawSprites();
  glEnable(GL_SCISSOR_TEST);
  glScissor(clientRect.pos.x ,
            winHeight-clientRect.rect.downRight.y-clientRect.pos.y ,
            clientRect.rect.downRight.x,
             winHeight-clientRect.pos.y);

  GuiInfo gi = GuiInfo( *guiInfo, e, game );
  createRect( 32.f, 32.f, &guiInfo->shapeDef->rect );
  gi.shapeDef->depth -= 1;

  gi.widgetData.spriteX.atlasInfo = ai;
  const uint32_t WIDTH = 15;
  float maxPosX = 0.f;
  float maxPosY = 0.f;
  //tabela 10 x Y
  int cell = 1;
  for( int y = 0; ; ++y ) {
    for( int x = 0; x < WIDTH; ++x ) {
      if(cell >= Y)
        goto endLoops;
      //wyswietle miniatury spriteów z atlasu w rozmiarze 32x32

      gi.shapeDef->pos = clientRect.pos + Vec2( ( x % WIDTH ) * 32.f, y * 32.f );
      maxPosX = std::max( maxPosX, gi.shapeDef->pos.x );
      maxPosY = std::max( maxPosY, gi.shapeDef->pos.y );

      gi.widgetData.spriteX.spriteId = cell;
      spriteX( e, game, inout, &gi );
      const GuiAction& guiAction = guiCmp->actionTest(e,gi.shapeDef);
      if( guiAction.hotInfo & GuiCmp::LEFT_RELEASED)
      //if(guiCmp->isActive(e) )
      {
        SpriteDef cursorDef;
        cursorDef.atlasFile = "img/by_ftorek.png";
        cursorDef.atlasInfoFile = "img/by_ftorek.tai";
        cursorDef.color = 0xffffffff;
        cursorDef.shape = cursorId;
        cursorDef.coordSpace = Render::SCREEN_COORD;
        cursorDef.textureName = ai->textureInfo[cell].filename;

        SpriteInfo * cursorSprite = game->getSpriteCmp()->
          overwrite(cursorId,cursorDef);
        game->getShapeCmp()->get(cursorId)->visible = true;
        game->editData.cursorTile = cell;
      }
      ++cell;
      //spriteX(e,game,action,guiInfo);
    }/*koniec for (x)*/
  }/*koniec for (y)*/
endLoops:

  guiInfo->widgetData.windowX.contentSizeX = maxPosX - clientRect.pos.x + 32.f;
  guiInfo->widgetData.windowX.contentSizeY = maxPosY - clientRect.pos.y + 32.f ;

  *guiInfo->shapeDef = shapeDef;
  //windowX( e, game, inout, guiInfo );
  /*
    rysuje tabele w oknie.
    jednak tabelka moze wystawać poza okienko, wiec trzeba rysowac mniejszą
    tabele: trzeba sie dopasowac do okna.
  */
  game->getRender()->sortAndDrawSprites();
  glDisable(GL_SCISSOR_TEST);
}




GuiInfo::GuiInfo() : state( false )
  , guiControl(), guiRespond()
  , shapeId(), styleId( )
  , styleInfo(), shapeDef()
  , widgetData()
{
}

GuiInfo::GuiInfo( const GuiInfo & gi, Entity e, Game * ) : entity( e )
  , guiControl( gi.guiControl ), guiRespond( gi.guiRespond )
  , controlType( gi.controlType ), respondName( gi.respondName )
  , state( gi.state ), shapeId( gi.shapeId ), styleId( gi.styleId )
  , styleInfo( gi.styleInfo ), shapeDef( gi.shapeDef )
  , widgetData( gi.widgetData )
{
  const parserNs::BaseParserItem * bpi = parser.getFunction( controlType.c_str() );
  if( bpi ) {
    guiControl = bpi->getPointer<GuiControl_pf>();
    assert( guiControl );
  }
  bpi = parser.getFunction( respondName.c_str() );
  if( bpi ) {
    guiRespond = bpi->getPointer<GuiRespond_pf>();
    assert( guiRespond );
  }

}

GuiInfo::GuiInfo( char * line [] ) : state( 0 )
  , guiControl(), guiRespond()
  , shapeId() , styleId( )
  , styleInfo(), shapeDef()
  , widgetData()
{
  entity = readEntityFormText( line[0] );

  char tmp [250] = {};
  sscanf( line[1], "%250s", tmp );
  controlType = tmp;
  memset( tmp, 0, 250 );
  sscanf( line[2], "%250s", tmp );
  respondName = tmp;

  shapeId = readEntityFormText( line[3] );
  styleId = readEntityFormText( line[4] );

}

GuiInfo::~GuiInfo()
{
}

std::string GuiInfo::getAsString()const {
  char tmp[250] = {};
  sprintf( tmp, "%i GuiInfo entity \n", entity.getId() );
  std::string out( tmp );
  memset( tmp, 0, 250 );

  if( controlType.length() > 0 ) {
    sprintf( tmp, " %s guiControlName \n", controlType.c_str() );
    out += tmp;
    memset( tmp, 0, 250 );
  }
  else {
    out += " 0 guiControlName \n";
  }
  if( respondName.length() > 0 )
  {
    sprintf( tmp, " %s guiRespondName\n ", respondName.c_str() );
    out += tmp;
    memset( tmp, 0, 250 );
  }
  else {
    out += " 0 guirespondName \n ";
  }

  out += getEntityAsText( shapeId );
  out += " shape\n ";
  out += getEntityAsText( styleId );
  out += " style\n";

  return out;
}

void GuiInfo::afterLoad( Game * game )
{
  if(styleId.getId() == 0 || shapeId.getId() == 0){
    game->getGuiCmp()->remove(entity);  //kasuje sam siebie - takie opakowane
      //delete this
    return;

  }
  styleInfo = game->getGuiStyleCmp()->getOrAdd( styleId );
  shapeDef = game->getShapeCmp()->getOrAdd( shapeId );
}

void guiCmpInputHandler( Game * game, InputDef * inputDef ,
                         SDL_Event * event , uint8_t * keyState )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  if( !event )  return ;
  switch( event->type )
  {
    case SDL_MOUSEBUTTONDOWN:
      switch( event->button.button ) {
        case SDL_BUTTON_WHEELUP:
          guiCmp->mouse |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->mouse |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break;//SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      switch( event->button.button ) {
        case SDL_BUTTON_WHEELUP:
          guiCmp->mouse |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->mouse |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break; // SDL_MOUSEBUTTONUP:
    case SDL_KEYDOWN:
      if( event->key.state == SDL_PRESSED )
      {
        guiCmp->keyEntered = event->key.keysym.sym;
        guiCmp->keyMod |= event->key.keysym.mod;
        guiCmp->unicode = event->key.keysym.unicode;
      }//if(event->key->state == SDL_PRESSED)

      break; //case SDL_KEYDOWN:
    case SDL_KEYUP:
      if( event->key.state == SDL_RELEASED )
      {
        guiCmp->unicode = 0;
        guiCmp->keyMod &= ~event->key.keysym.mod;
        guiCmp->keyEntered = 0;
      }
      break;  //case SDL_KEYUP:
  }

}


int32_t GuiCmp::loadText( const char * filename, std::vector<Entity>& unit )
{
  int32_t r = BaseType::loadText( filename, unit );
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  game->getInputCmp()->overwrite( e, inputDef );
  return r;
}

GuiCmp::GuiCmp( Game * game ) : BaseType( game )
  , hot( ), active( ), hotDepth( 0 )
  , kbdItem( gameEntity )
  , lastFocused( ) , mouse( 0 ), keyEntered()
  , keyMod()
  , mark( 0 ), cursor( 0 )
  , locked( 0 ), lockReason()
  , hotShape( 0 )
  , activeDetails ()
{
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  e = game->createEntity();
  game->getInputCmp()->add( e, inputDef );

  hotShape = new ShapeDef();

  parser.addFun4( &editX, "editX" );
  parser.addFun4( &consoleX, "consoleX" );
  parser.addFun4( &windowX, "windowX" );
  parser.addFun4( &drawMapTiles, "drawMapTiles" );
}
GuiCmp::~GuiCmp()
{
  delete hotShape;
}

void GuiCmp::initFrame(ShapeDef * windowShape)
{
  update();
  actionTest( gameEntity, windowShape );
}

static inline void enableSDLEditMode() {
  SDL_EnableKeyRepeat( 5000, 300 );
  SDL_EnableUNICODE( 1 );
}
void GuiCmp::focusNext( GuiAction * ai )
{
  kbdItem = nullEntity;
  keyEntered = 0;
  ai->hotInfo &= ~FOCUSED;
  cursor = 0x7fff;
  mark = 0;
}

void GuiCmp::focusPrev( GuiAction * ai )
{
  kbdItem = lastFocused;
  keyEntered = 0;
  ai->hotInfo &= ~FOCUSED;
  cursor = 0x7fff;
  mark = 0;
}

GuiAction GuiCmp::actionTest( Entity e , ShapeDef * shapeDef )
{
  assert( e.getId() );
  GuiAction action;
  //gorący zablokował swój focus - nikt inngy go nie dostanie
  if( locked.getId() != 0 && hot != e )
    return action;

  uint32_t & out = action.hotInfo;
  out = 0;
  if( nullEntity == kbdItem )
  {
    //przejmuje focus
    kbdItem = e;
  }

  if( e == kbdItem ) {
    out |= FOCUSED;
    enableSDLEditMode();

    switch( keyEntered )
    {
      case SDLK_TAB:
        if( SDLK_LSHIFT == keyMod )
        {
          focusPrev( &action );
          break;
        }
        focusNext( &action );
        break;

      case SDLK_RETURN:
        break;

    }//koniec switch'a
    action.key = keyEntered;
    action.unicode = unicode;
    action.keyMod = keyMod;
  }//koniec if'a

  lastFocused = e;

  XY<int32_t> mousePos;
  uint8_t mouseState = SDL_GetMouseState( &mousePos.x, &mousePos.y );
  //ShapeDef * shapeDef = game->getShapeCmp()->get( e );

  if( !shapeDef )
    return GuiAction();

  if( locked.getId() != 0 ) {
    //jakaś kontrolka się zablokowała - bedzie otrzymywała focus dopuki
    //go nieodblokuje
    goto makeHotLabel;
  }
  //dzieki goto otrzymałem rozbudeoane if( ... && (...|| ... || ... ) )
  //kursor wewnatrz kontrolki
  if( shapeDef->isInside( mousePos ) ) {
    //zadna nie jest goraca wiec śmiało można klikać
    //puts(" shapeDef->isInside( mousePos ) ");
    if( nullEntity == hot ) {
      //! moje pierwsze pierwsze  użycie goto
      goto makeHotLabel;
    }
    /*
        if( topId != nullEntity){ //jest coś na topie
          if( e != topId && hotShape->isInside(mousePos) )
          {  //no i musz w topowym okienku, ale sprawdzam inne okno(nie topowe)
              //niemoze ono dostać focusa
            goto endLabel;
          }
        }

    //    printf("hot depth %hi ; shapeDef->depth  %hi\n",hotDepth, shapeDef->depth );
        if( hotDepth > shapeDef->depth ) {
          //puts("hotDepth > shapeDef->depth");
          // glebokosc obecnie najechanej kontrolki jest mniejsza niz wcześniejszej
          // goracej wiec można śmiało ją zaznaczyć
          // ale juz wcześniej kontrolko mogła zostać gorąca a niemogą być dwie
          // gorące w jednej klatce
          //goto makeHotLabel;
          //dlatego ^ to wyjebałem, teraz tylko ustawie głębokość, a odpowiednia
          //kontrolka bedzie gorąca w następnej klatce
          hotDepth = shapeDef->depth;
          goto endLabel;
        }else if (hotDepth == shapeDef->depth ){
          //printf(" hotDepth % hi == %hi shapeDef->depth \n",
          //         hotDepth, shapeDef->depth );

          //taka sama głębokość wiec można aktywować
          //zapewne w poprzedniej klatce spełniony był pierwszy if ^
          //hmm.. gdy dwa okna nają taką samą głębokośc to o widoczności decyduje
          //kolejność rysowania

          goto makeHotLabel;
        }
    */
    //if( e != hot )
    { //nie porownuje samego ze soba

      //jesli mysz nie jest w środku gorącego i jest w środku obecnego
      if( !hotShape->isInside( mousePos ) ) {
        //puts("!hotShape->isInside( mousePos ) ");
        goto makeHotLabel;
      }
      else { //mysz jest w środku obu prostokątów, sprawdzam który jest płycej
        //na topie jest ten aktywny; ten na topie ma pierszeństwo
        if( e == active ) {
          //puts("if(e == active)");
          goto makeHotLabel;
        }
        else { //nie na topie zaznaczam tego który jest płycej( mniejszy depth)
          if( hotDepth > shapeDef->depth ) {
            //puts("hotDepth > shapeDef->depth");
            hotDepth = shapeDef->depth;
            goto makeHotLabel;
          }
          else if( hotDepth == shapeDef->depth ) {
            //puts("hotDepth == shapeDef->depth");
            //do tego ifa wchodzi gdy:
            // 1) wykonał sie wcześniejszy if we wcześniejszej klatce
            // 2) dwie kontrolki mają taką samą głębokość i na wierzchu bedzie
            //    ta która jest poźniej rysowana
            // 3) e == hot
            goto makeHotLabel;
          }
        }
      }
    }
  }

  goto endLabel;

makeHotLabel:
//  if(makeHot)
  {
    //puts("makeHot");
    uint8_t prevMouseState = game->getInputCmp()->getPrevFrameMouseState();

    hot = e;
    hotDepth = shapeDef->depth;
    *hotShape = *shapeDef;
    //gorąca kontrolka powinna wyskakiwać na wierzch

    out |= HOVER;

    //jesli isMouse.. da 1 to 1*MASK = MASK; jesli 0 to 0*MASK = 0
    out |= isMouseButtonReleased( SDL_BUTTON_LEFT, prevMouseState, mouseState )
           * LEFT_RELEASED;
    out |= isMouseButtonReleased( SDL_BUTTON_MIDDLE, prevMouseState, mouseState )
           * MIDDLE_RELEASED;
    out |= isMouseButtonReleased( SDL_BUTTON_RIGHT, prevMouseState, mouseState )
           * RIGHT_RELEASED;

    out |= ( bool( SDL_BUTTON( SDL_BUTTON_LEFT ) & mouseState ) ) * LEFT_DOWN;
    out |= ( bool( SDL_BUTTON( SDL_BUTTON_MIDDLE ) & mouseState ) ) * MIDDLE_DOWN;
    out |= ( bool( SDL_BUTTON( SDL_BUTTON_RIGHT ) & mouseState ) ) * RIGHT_DOWN;


    int8_t wheel = game->getInputCmp()->getWheelDelta();
    if( wheel > 0 ) out |= WHEEL_UP;
    if( wheel < 0 ) out |= WHEEL_DOWN;

    if( out & ( ANY_RELEASED | WHEEL_UP | WHEEL_DOWN | ANY_DOWN ) ) {

      active = e;
      activeDetails = action;

      kbdItem = e;
      enableSDLEditMode();

      cursor = 0x7fff;
      mark = 0;
    }
  }

endLabel:

  //printf("hot: %u active: %u \n",hot.getId(),active.getId() );
  return action;

}

void GuiCmp::drawGui()
{
  ShapeCmp * shapeCmp = game->getShapeCmp();
  for( int i = 0; i < ( int )records.size(); ++i ) {
    GuiInfo * guiInfo = records[i];
    Entity & e = guiInfo->entity;

    if( guiInfo->guiControl ) {
      WidgetAction action;
      const int16_t oldDepth = guiInfo->shapeDef->depth;
      if( active == e ) { // jesli jest aktywne to wylatuje na wierzch
        guiInfo->shapeDef->depth = -3200;  // ta wartość, hmm... może kiedyś ja będe
        //odczytywał z pliku, w każdym razie jest to głębokość na jaką wyskoczy
        //okienko aktywne, na topie. Praktycznie minimalna wartość
      }
      action = actionTest( guiInfo->entity, guiInfo->shapeDef );
      //action test mogł aktywować kontrolke, ponownie sprawdzam
      if( active == e ) {
        guiInfo->shapeDef->depth = -3200;
      }
      guiInfo->guiControl( guiInfo->entity, game, &action, guiInfo );
      if( guiInfo->guiRespond ) {
        guiInfo->guiRespond( guiInfo->entity, game, &action, guiInfo );
      }
      guiInfo->shapeDef->depth = oldDepth;
    }
  }/*koniec for (i)*/
  /*
  */
}

void GuiCmp::update()
{
  mouse &= ~( WHEEL_DOWN | WHEEL_UP );

  if( SDLK_TAB == keyEntered ) {
    kbdItem = notFocused;
    SDL_EnableKeyRepeat( 0, 0 );
    SDL_EnableUNICODE( 0 );
  }

  keyEntered = 0;
  unicode = 0;

  active = nullEntity;
}

void GuiCmp::setVisibilityAll( bool v )
{
  ShapeCmp * shapeCmp = game->getShapeCmp();
  for( int i = 0; i < ( int )records.size(); ++i ) {
    GuiInfo * guiInfo = records[i];
    ShapeDef * shapeDef = shapeCmp->get( guiInfo->entity );
    if( shapeDef )
      shapeDef->visible = v;
  }/*koniec for (i)*/
}

