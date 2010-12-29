#include "common.h"
#include "widget.h"
#include "main.h"

bool makeControlMovable( GuiAction & a, const Vec2 & mouseDelta,
                         GuiCmp::ControlFlag whenStart )
{
  if( a )
  {
    if( a.info & whenStart ) {
      a.guiInfo->state  |= GuiInfo::MOVING;
    }
    else {
      a.guiInfo->state &= ~GuiInfo::MOVING ;
    }
  }
  if( a.guiInfo->state & GuiInfo::MOVING ) {
    a.shapeDef->pos += mouseDelta;
    if( a.textInfo ) {
      a.textInfo->position += mouseDelta;
      //po przemieszczeniu kursor poza kontrolką?
      XY<int32_t> mousePos;
      SDL_GetMouseState( &mousePos.x, &mousePos.y );
      if( ! a.shapeDef->isInside( mousePos ) )
        a.guiInfo->state &= ~GuiInfo::MOVING;
    }
  }
  return a.guiInfo->state & GuiInfo::MOVING;
}
bool makeControlMovable( GuiAction & a, const XY<int32_t>& mouseDelta,
                         GuiCmp::ControlFlag whenStart )
{
  return makeControlMovable( a, Vec2( mouseDelta ), whenStart );
}

/*
bool makeControlResizable( GuiAction & a, const Vec2 & mouseDelta,
                           GuiCmp::ControlFlag whenStart )
{
  if( a )
  {
    if( a.info & whenStart ) {
      a.guiInfo->state |= GuiInfo::RESIZING;
    }
    else {
      a.guiInfo->state &= ~GuiInfo::RESIZING;
    }
  }
  if( a.guiInfo->state & GuiInfo::RESIZING) {
    //a.shapeDef->pos += mouseDelta;
    a.shapeDef->rect.upRight += Vec2( mouseDelta.x, 0.f );
    a.shapeDef->rect.downLeft += Vec2( 0.f, mouseDelta.y );
    a.shapeDef->rect.downRight += Vec2( mouseDelta );

    if( a.textInfo ) {
      //a.textInfo->position += mouseDelta;
      //po przemieszczeniu kursor poza kontrolką?
      XY<int32_t> mousePos;
      SDL_GetMouseState( &mousePos.x, &mousePos.y );
      if( ! a.shapeDef->isInside( mousePos ) )
        a.guiInfo->state &= ~GuiInfo::RESIZING;
    }
  }
  return a.guiInfo->state & ~GuiInfo::RESIZING;
}
*/
bool makeControlResizable( GuiAction & a, const XY<int32_t>& mouseDelta,
                           GuiCmp::ControlFlag whenStart )
{
  return makeControlResizable( a, Vec2( mouseDelta ), whenStart );
}

GuiAction labelX( const Entity e, Game * game )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->get( e );

  //troche hujnia z tym, ale jest rozwiazanie, poprostu niech sobie render to
  //rysuje, a jak bede chcaił coś zmienić to zmienie i tak obiekt SpriteInfo
  //
  shapeDef->visible = true;
  spriteCmp->draw( e, *shapeDef);
  shapeDef->visible = false;

  textInfo->position = shapeDef->pos;
  textInfo->visible = true;
  textCmp->draw( e );
  textInfo->visible = false;

  return action;
}

GuiAction consoleX( const Entity e, Game * game ) {

  ShapeCmp * shapeCmp = game->getShapeCmp();
  GuiCmp * guiCmp = game->getGuiCmp();

  GuiAction gameAction = guiCmp->actionTest( gameEntity );
  GuiAction a = guiCmp->actionTest( e );
  ShapeDef* &sh = a.shapeDef;
  const XY<int32_t> winDim = XY<int32_t>( game->getRender()->getWindowDim() );
  //const int maxHeight = winDim.y * 0.5f;
  const int maxHeight = 80;

  a.textInfo = game->getTextCmp()->get( e );
  TextInfo qwahaey3__4334;
  if(!a.textInfo){
    a.textInfo = &qwahaey3__4334;
  }
  if(! a.shapeDef ){
    ShapeDef sd;
    sd.pos = Vec2(100.f,100.f);
    sd.rect.upRight = Vec2(0.f,100.f);
    sd.rect.downLeft =Vec2(100.f,0.f);
    sd.rect.downRight=Vec2(100.f,100.f);
    sd.visible = true;
    a.shapeDef = shapeCmp->add(e,sd);
  }

  if( !a.shapeDef->visible )
  { // konsolka niewidoczna
    a.textInfo->visible = false;
    if( gameAction.key == SDLK_BACKQUOTE )  //pokazuje kontrolke
    { //wcisnieto ` i nalezy ja pokazac
      //guiCmp->makeFocused(e);
      //guiCmp->focusNext( &a );

      sh->pos = Vec2( 0.f, 0.f );
      sh->rect.upLeft = Vec2( 0.f, 0.f );
      sh->rect.upRight = Vec2( winDim.x, 0.f );

      sh->rect.downLeft = Vec2( 0.f, 0.f );
      sh->rect.downRight = Vec2( winDim.x, 0.f );


      a.textInfo->visible = true;
      //a.textInfo->position = Vec2(10.f,10.f);
      //a.textInfo->color = makeARGB(255,200,10,10);
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
    //a.textInfo = game->getTextCmp()->get(e);
    //a.textInfo->visible = true;
    //a.textInfo->position = Vec2(10.f,10.f);
    //a.textInfo->color = makeARGB(255,200,10,10);

    if( gameAction.key == SDLK_BACKQUOTE || a.key == SDLK_BACKQUOTE )
    { //pora sie schowac
      sh->visible = false;
      a.textInfo->visible = false;
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

  return a;
}

GuiAction windowX( const Entity e, Game * game )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->get( e );

  if(! shapeDef ){
    ShapeDef sd;
    sd.pos = Vec2(100.f,100.f);
    sd.rect.upRight = Vec2(0.f,100.f);
    sd.rect.downLeft =Vec2(100.f,0.f);
    sd.rect.downRight=Vec2(100.f,100.f);
    sd.visible = true;
    shapeDef = game->getShapeCmp()->add(e,sd);
  }

  //próba walnięcia tekstury :P
/*
  static Entity s_entity = game->createEntity();
  bool isSpriteAdded = game->getSpriteCmp()->get(s_entity);
*/
  int mouseX,mouseY;
  uint8_t mouseS = SDL_GetMouseState(&mouseX,&mouseY);
  Vec2 mousePos(mouseX,mouseY);

  ShapeDef barShape;
  const float& barHeight = action.guiInfo->barHeight;
  const float& borderWidth = action.guiInfo->borderWidth;

  barShape.pos = shapeDef->pos - Vec2(0.f,barHeight);
  barShape.rect.upRight = Vec2( shapeDef->rect.upRight.x, 0.f );
  barShape.rect.downLeft = Vec2( 0.f, barHeight );
  barShape.rect.downRight = Vec2( shapeDef->rect.upRight.x, barHeight );
  barShape.visible = true;
  barShape.depth = action.guiInfo->depth;

  GuiInfo* &guiInfo = action.guiInfo;

  if(guiInfo->barSpriteEntity.getId() == 0){
    game->getSpriteCmp()->drawRect(barShape,makeARGB(255,20,20,180),
                                  action.guiInfo->depth);

  }else{
    game->getSpriteCmp()->forceDraw(guiInfo->barSpriteEntity,barShape);
  }

  ShapeDef borderTop,borderBottom,borderLeft,borderRight;
  borderTop.pos = barShape.pos - Vec2(borderWidth,borderWidth);
  createRect(shapeDef->rect.upRight.x + 2*borderWidth,borderWidth,
             &borderTop.rect);


  createRect(shapeDef->rect.downRight.x + 2*borderWidth,borderWidth,
             &borderBottom.rect);
  borderBottom.pos = shapeDef->pos + shapeDef->rect.downLeft
    - Vec2(borderWidth,0.f);


  createRect(borderWidth,shapeDef->rect.downRight.y+2*borderWidth + barHeight,
             &borderRight.rect);
  borderRight.pos = shapeDef->pos + shapeDef->rect.upRight
    - Vec2(0.f,barHeight+borderWidth );

  borderLeft.pos = barShape.pos - Vec2(borderWidth,borderWidth);
  createRect(borderWidth,shapeDef->rect.downLeft.y+2*borderWidth + barHeight,
             &borderLeft.rect);


  if(guiInfo->borderSpriteEntity.getId() == 0)
  {
    game->getSpriteCmp()->drawRect(borderTop,makeARGB(255,210,230,230),
                                 action.guiInfo->depth-1);
    game->getSpriteCmp()->drawRect(borderBottom,makeARGB(255,210,230,230),
                                 action.guiInfo->depth-1);
    game->getSpriteCmp()->drawRect(borderRight,makeARGB(255,210,230,230),
                                 action.guiInfo->depth-1);
    game->getSpriteCmp()->drawRect(borderLeft,makeARGB(255,210,230,230),
                                 action.guiInfo->depth-1);
  }else{
    spriteCmp->forceDraw(guiInfo->borderSpriteEntity, borderTop );
    spriteCmp->forceDraw(guiInfo->borderSpriteEntity, borderBottom );
    spriteCmp->forceDraw(guiInfo->borderSpriteEntity, borderRight );
    spriteCmp->forceDraw(guiInfo->borderSpriteEntity, borderLeft );
  }

  InputCmp * input = game->getInputCmp();
  if(SDL_BUTTON(SDL_BUTTON_LEFT) & mouseS)
  {
    if( barShape.isInside(mousePos) )
    {
      action.guiInfo->state |= GuiInfo::MOVING;
    }
    //czyli jak jest kursor w ramce
    else{
      if( borderTop.isInside(mousePos) )
      {
        action.guiInfo->state |= GuiInfo::RESIZING_TOP;
      }
      if( borderLeft.isInside(mousePos) )
      {
        action.guiInfo->state |= GuiInfo::RESIZING_LEFT;
      }
      if( borderRight.isInside(mousePos) )
      {
        action.guiInfo->state |= GuiInfo::RESIZING_RIGHT;
      }
      if( borderBottom.isInside(mousePos) )
      {
        action.guiInfo->state |= GuiInfo::RESIZING_BOTTOM;
      }
    }
  }

  if(!(SDL_BUTTON(SDL_BUTTON_LEFT) & mouseS) ){
    action.guiInfo->state &= ~GuiInfo::MOVING;
    action.guiInfo->state &= ~GuiInfo::ALL_RESIZING;
  }
  if (action.guiInfo->state & GuiInfo::MOVING )
  {
    shapeDef->pos += Vec2(game->getInputCmp()->getMousePosDelta() );
  }
  else
  {
    if( (action.guiInfo->state & GuiInfo::RESIZING_TOP ) )
    {
      Vec2 dy (0.f,input->getMousePosDelta().y);
      shapeDef->pos += dy;
      shapeDef->rect.downLeft -= dy;
      shapeDef->rect.downRight -= dy;
    }
    if( (action.guiInfo->state & GuiInfo::RESIZING_BOTTOM ) )
    {
      Vec2 dy (0.f,input->getMousePosDelta().y);
      shapeDef->rect.downLeft += dy;
      shapeDef->rect.downRight += dy;
    }
    if( (action.guiInfo->state & GuiInfo::RESIZING_LEFT ) )
    {
      Vec2 dx( input->getMousePosDelta().x,0.f);
      shapeDef->pos += dx;
      shapeDef->rect.upRight -= dx;
      shapeDef->rect.downRight -= dx;
    }
    if( (action.guiInfo->state & GuiInfo::RESIZING_RIGHT ) )
    {
      Vec2 dx( input->getMousePosDelta().x,0.f);
      shapeDef->rect.upRight += dx;
      shapeDef->rect.downRight += dx;
    }
    float width = shapeDef->rect.upRight.x - shapeDef->rect.upLeft.x;
    if(width < 3* borderWidth){ //rozszerzam
      shapeDef->rect.upRight.x = 3* borderWidth;
      shapeDef->rect.downRight.x = 3* borderWidth;
    }
    float height = shapeDef->rect.downLeft.y - shapeDef->rect.upLeft.y;
    if(height < 3* borderWidth){
      shapeDef->rect.downRight.y = 3* borderWidth;
      shapeDef->rect.downLeft.y = 3* borderWidth;
    }


  }//else

  if(textInfo && barHeight ){
    textInfo->position = barShape.pos;
    textCmp->draw( e );
  }

  return action;

}
void windowXCallback( Game * game, InputDef * inputDef, SDL_Event * event,
                           uint8_t * keyState )
{
  if(!event)
    return;
  Entity e = inputDef->entity;
  if( game->getGuiCmp()->isFocused(e) )
  {//jest focus
    GuiInfo * guiInfo = game->getGuiCmp()->get(e );
    ShapeDef * shapeDef = game->getShapeCmp()->get( e );

    int mouseX,mouseY;
    uint8_t mouseS = SDL_GetMouseState(&mouseX,&mouseY);
    Vec2 mousePos(mouseX,mouseY);
    ShapeDef barShape;
    static const float barHeight = 15.f;
    barShape.rect.upRight = Vec2( shapeDef->rect.upRight.x, 0.f );
    barShape.rect.downLeft = Vec2( 0.f, barHeight );
    barShape.rect.downRight = Vec2( shapeDef->rect.downRight.x, barHeight );

    if( barShape.isInside(mousePos) ){

      if( SDL_BUTTON(SDL_BUTTON_LEFT) & mouseS){
        guiInfo->state |= GuiInfo::MOVING;
      }
    }
    if(!(SDL_BUTTON(SDL_BUTTON_LEFT) & mouseS) ){
      guiInfo->state &= ~GuiInfo::MOVING;
    }
    if (guiInfo->state )
    {
      shapeDef->pos += Vec2(game->getInputCmp()->getMousePosDelta() );
    }
  }

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

GuiAction editX( const Entity e , Game * game )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->getOrAdd( e );

  if(!shapeDef)
    return GuiAction();

//  shapeDef->visible = true;
//  spriteCmp->draw( e );
//  shapeDef->visible = false;

  std::string & str = textInfo->text;

  textInfo->position = shapeDef->pos;
  textInfo->visible = true;
  textCmp->draw( e );
  textInfo->visible = false;

  if( action.info & GuiCmp::FOCUSED ) {

    int16_t & cursor = guiCmp->cursor;
    int16_t & mark = guiCmp->mark;


    cursor = std::min( int16_t( str.size() ), cursor );

    switch( action.key )
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
        if( action.keyMod & KMOD_CTRL ) {
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
          if( !( action.keyMod & KMOD_SHIFT ) ) {
            mark = 0;
          }

        }
        else if( action.keyMod & KMOD_SHIFT ) {
          ++mark;
        }
        else {

          mark = 0;
        }
        break;
      case SDLK_RIGHT:
        cursor = std::min( int16_t( str.size() ), ++guiCmp->cursor );
        if( action.keyMod & KMOD_SHIFT )
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
        str.insert( guiCmp->cursor++, 1, action.unicode );

        //str += action.key;
        break;
    }//switch(action.key)

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

    spriteCmp->drawRect( def, textInfo->color, textInfo->depth );

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

      spriteCmp->drawRect( def, makeARGB( 150, 220, 30, 30 ), textInfo->depth );

    }

  }

  return action;
}

GuiAction controlPaneX( const Entity e , Game * game )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->get( e );

  shapeDef->visible = true;
  spriteCmp->draw( e, *shapeDef );
  shapeDef->visible = false;

  return action;

}
