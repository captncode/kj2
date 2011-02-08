#include "guicmp.h"
#include "main.h"
#include "motioncmp.h"
#include "inputcmp.h"
#include "parser.h"
#include "widget.h"
#include "render.h"

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

/*!  scroll - czy są scroll bary?
    0 - brak
    1 - pionowy
    2 - poziomy
    3 - pionowy i poziomy
*/
void calcWindowXUserRect( ShapeDef * shapeDef, GuiStyleInfo * gsi,
                          ShapeDef * out, int gotScroll )
{
  WindowXStyle * ws = &gsi->windowX;
  out->pos = shapeDef->pos + Vec2( ws->borderWidth,
                                   ws->borderWidth + ws->barHeight );
  float scrlVerS = 0.f;
  float scrlHorS = 0.f;
  if( gotScroll & 1 ) {
    scrlVerS = gsi->sliderX.width;
  }
  if( gotScroll & 2 ) {
    scrlHorS = gsi->sliderX.width;
  }

  createRect( shapeDef->rect.upRight.x - shapeDef->rect.upLeft.x -
              2 * ws->borderWidth - scrlVerS,
              shapeDef->rect.downLeft.y - shapeDef->rect.upLeft.y -
              2 * ws->borderWidth - ws->barHeight - scrlHorS, &out->rect );
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
#ifdef COMMENT
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

  if( guiCmp->isFocused( e ) ) {
    GuiAction
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
        str.insert( guiCmp->cursor++, 1, action->keyUnicode );

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
#endif
}




GuiInfo::GuiInfo()
{
  //memcpy(this,&gi, );
  memset( this, 0, sizeof( *this ) );

}

GuiInfo::GuiInfo( const GuiInfo & gi, Entity e, Game * game )
{
  memcpy( this, &gi, sizeof( *this ) );
  if( !shapeDef ) {
    shapeDef = game->shapeCmp->get( gameEntity ); //domyslna zaslepka
  }
  if( !styleInfo ) {
    styleInfo = game->guiStyleCmp->getOrAdd( gameEntity );
    assert( styleInfo );
  }

}

GuiInfo::GuiInfo( char * line [] ) :
  styleInfo(), shapeDef(), widgetProcedure()
{
  entity = readEntityFormText( line[0] );

  Entity shapeId = readEntityFormText( line[1] );
  Entity styleId = readEntityFormText( line[2] );

  if( styleId.getId() && shapeId.getId() ) {
    shapeDef = g_game->getShapeCmp()->getOrAdd( shapeId );
    styleInfo = g_game->getGuiStyleCmp()->getOrAdd( styleId );
  }
}

GuiInfo::~GuiInfo()
{
}

std::string GuiInfo::getAsString()const {
  char tmp[250] = {};
  std::string out( getEntityAsText( entity ) );
  out += " GuiInfo\n ";

  out += getEntityAsText( shapeDef->entity );
  out += " shapeId\n ";
  out += getEntityAsText( styleInfo->entity );
  out += " styleId\n";

  return out;
}

void GuiInfo::afterLoad( Game * game )
{
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
          guiCmp->state.buttonState |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->state.buttonState |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break;//SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      switch( event->button.button ) {
        case SDL_BUTTON_WHEELUP:
          guiCmp->state.buttonState |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->state.buttonState |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break; // SDL_MOUSEBUTTONUP:
//    case SDL_MOUSEMOTION:
//      guiCmp->state.prevMouseX = guiCmp->state.mouseX;
//      guiCmp->state.prevMouseY = guiCmp->state.mouseY;
//      guiCmp->state.mouseX = event->motion.x;
//      guiCmp->state.mouseY = event->motion.y;
//      break;

    case SDL_KEYDOWN:
      if( event->key.state == SDL_PRESSED )
      {
        guiCmp->state.keyEntered = event->key.keysym.sym;
        guiCmp->state.keyMod |= event->key.keysym.mod;
        guiCmp->state.keyUnicode = event->key.keysym.unicode;
      }//if(event->key->state == SDL_PRESSED)

      break; //case SDL_KEYDOWN:
    case SDL_KEYUP:
      if( event->key.state == SDL_RELEASED )
      {
        guiCmp->state.keyUnicode = 0;
        guiCmp->state.keyMod &= ~event->key.keysym.mod;
        guiCmp->state.keyEntered = 0;
      }
      break;  //case SDL_KEYUP:
  }


  // pozniej to wykonywac tylko jak przyjdzie dpowiedni event dla mychy
//  guiCmp->state.prevMouseX = guiCmp->state.mouseX;
//  guiCmp->state.prevMouseY = guiCmp->state.mouseY;
  int mx, my;
  uint8_t buttonsSt = SDL_GetMouseState( &mx, &my );
//  guiCmp->state.mouseX = mx;
//  guiCmp->state.mouseY = my;

  uint8_t prevMouseState = game->getInputCmp()->getPrevFrameMouseState();

  guiCmp->state.buttonState = 0;
  //jesli isMouse.. da 1 to 1*MASK = MASK; jesli 0 to 0*MASK = 0
  guiCmp->state.buttonState |=
    isMouseButtonReleased( SDL_BUTTON_LEFT, prevMouseState, buttonsSt )
    * GuiCmp::LEFT_RELEASED;
  guiCmp->state.buttonState |=
    isMouseButtonReleased( SDL_BUTTON_MIDDLE, prevMouseState, buttonsSt )
    * GuiCmp::MIDDLE_RELEASED;
  guiCmp->state.buttonState |=
    isMouseButtonReleased( SDL_BUTTON_RIGHT, prevMouseState, buttonsSt )
    * GuiCmp::RIGHT_RELEASED;

  guiCmp->state.buttonState |=
    ( bool( SDL_BUTTON( SDL_BUTTON_LEFT ) &buttonsSt ) ) * GuiCmp::LEFT_DOWN;
  guiCmp->state.buttonState |=
    ( bool( SDL_BUTTON( SDL_BUTTON_MIDDLE ) &buttonsSt ) ) * GuiCmp::MIDDLE_DOWN;
  guiCmp->state.buttonState |=
    ( bool( SDL_BUTTON( SDL_BUTTON_RIGHT ) & buttonsSt ) ) * GuiCmp::RIGHT_DOWN;

}


int32_t GuiCmp::loadText( const char * filename, std::vector<Entity>& unit )
{
  int32_t r = BaseType::loadText( filename, unit );
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  game->getInputCmp()->overwrite( guiId, inputDef );
  return r;
}

GuiCmp::GuiCmp( Game * game ) : BaseType( game )
  , hot( ), active( ), hotDepth( 0 )
  , kbdItem( gameEntity )
  , lastTested( )
  , locked( 0 ), lockReason()
  , hotShape( 0 )
{
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  guiId = game->createNextEntity();
  game->getInputCmp()->add( guiId, inputDef );

  hotShape = new ShapeDef[2];
  testRegion = hotShape + 1;
  createRect( 1e36f, 1e36f, &testRegion->rect );
  translateQuad( testRegion->rect, Vec2( -( 1e36f * 0.5f ), -( 1e36f * 0.5f ) ),
                 &testRegion->rect );

}
GuiCmp::~GuiCmp()
{
  delete [] hotShape;
}

void GuiCmp::initFrame( ShapeDef * windowShape )
{
  prevState = state;
  const XY<int32_t>& prevPos = game->inputCmp->getPrevFrameMousePos();
  state.prevMouseX = prevPos.x;
  state.prevMouseY = prevPos.y;
  const XY<int32_t>& pos = game->inputCmp->getMousePos();
  state.mouseX = pos.x;
  state.mouseY = pos.y;



  if( SDLK_TAB == state.keyEntered ) {
    kbdItem = notFocused;
    SDL_EnableKeyRepeat( 0, 0 );
    SDL_EnableUNICODE( 0 );
  }

  state.keyEntered = 0;
  state.keyUnicode = 0;

  if( state.buttonState & ( ~ANY_DOWN | WHEEL_DOWN | WHEEL_UP ) ) {
    active = nullEntity;
  }

  justReleased = nullEntity;
  //hot = nullEntity;

  actionTest( gameEntity, windowShape );
}

static inline void enableSDLEditMode() {
  SDL_EnableKeyRepeat( 5000, 300 );
  SDL_EnableUNICODE( 1 );
}
void GuiCmp::focusNext( )
{
  kbdItem = nullEntity;
  state.keyEntered = 0;
  state.cursor = 0x7fff;
  state.mark = 0;
}

void GuiCmp::focusPrev( )
{
  kbdItem = lastTested;
  state.keyEntered = 0;
  state.cursor = 0x7fff;
  state.mark = 0;
}

GuiCmp::TestValue GuiCmp::actionTest( Entity e , ShapeDef * shapeDef )
{
  assert( e.getId() );
  assert( shapeDef );

  lastTested = e;

  XY<int32_t> mousePos( state.prevMouseX + mouseTranslation.x,
                        state.prevMouseY + mouseTranslation.y );

  //gorący zablokował swój focus - nikt inngy go nie dostanie
  if( locked.getId() != 0 && hot != e ) {
    //return makeHot(e,shapeDef);
    return LOCKED_HOT;
  }


  if( notFocused == kbdItem )
  {
    kbdItem = e; //przejmuje focus
  }

  if( e == kbdItem ) {
    enableSDLEditMode();

    switch( state.keyEntered )
    {
      case SDLK_TAB:
        if( SDLK_LSHIFT == state.keyMod )
        {
          focusPrev( );
          break;
        }
        focusNext( );
        break;

      case SDLK_RETURN:   //kontrolka obsluzy jak bedzie chciała
        break;

    }//koniec switch'a

  }//koniec if'a

  if( locked.getId() != 0 ) {
    //jakaś kontrolka się zablokowała - bedzie otrzymywała focus dopuki
    //go nieodblokuje
    return makeHot( e, shapeDef );
    //return BY_ME_LOCKED;
  }

  //kursor wewnatrz kontrolki
  if( testRegion->isInside( mousePos ) && shapeDef->isInside( mousePos ) ) {

    //jezeli inna kontrolka jest goraca oraz
    //jesli mysz nie jest w środku gorącego i jest w środku obecnego
    if( hot != e && !hotShape->isInside( mousePos ) ) {
      //puts("!hotShape->isInside( mousePos ) ");
      return makeHot( e, shapeDef );
    }

    if( shapeDef->depth < hotDepth ) {
      //puts("hotDepth > shapeDef->depth");
      hotDepth = shapeDef->depth;
      return makeHot( e, shapeDef );
    }
    if( hotDepth == shapeDef->depth ) {
      //puts("hotDepth == shapeDef->depth");
      //do tego ifa wchodzi gdy:
      // 1) wykonał sie wcześniejszy if we wcześniejszej klatce
      // 2) dwie kontrolki mają taką samą głębokość i na wierzchu bedzie
      //    ta która jest poźniej rysowana
      // 3) e == hot
      if( hot == e || hot == nullEntity ) //nie mogą być 2 gorące w jednej klatce
        return makeHot( e, shapeDef );
    }


  }

//  printf( "hot: %i active: %i focused: %i\n", hot.getId(), active.getId(),
//          kbdItem.getId() );
  //clearIfHot(e);
  return COLD;
}

GuiCmp::TestValue GuiCmp::makeHot( Entity e, ShapeDef * shapeDef )
{
  clearIfActive( e );

  //printf("makeHot: %i\n",e.getId() );
  hot = e;
  hotDepth = shapeDef->depth;
  *hotShape = *shapeDef;

  uint32_t & out = state.buttonState;

  int8_t wheel = game->getInputCmp()->getWheelDelta();
  if( wheel > 0 )
    out |= WHEEL_UP;
  else if( wheel < 0 )
    out |= WHEEL_DOWN;
  else
    out &= ~( WHEEL_UP | WHEEL_DOWN );

  if(( out & ( WHEEL_UP | WHEEL_DOWN | ANY_DOWN ) ) &&
      ( active == nullEntity ) )
  {
    active = e;

    kbdItem = e;
    enableSDLEditMode();

    state.cursor = 0x7fff;
    state.mark = 0;
    return ACTIVATED;
  }
  else if( out & ~( WHEEL_UP | WHEEL_DOWN | ANY_DOWN ) && active == e )
  { // pusszczono klawisz, juz kółko sie nie kręci
    justReleased = e;
    return RELEASED;
  }
  else if( active == e ) {
    return ACTIVE;
  }

//  printf( "hot: %i active: %i focused: %i\n", hot.getId(), active.getId(),
//          kbdItem.getId() );
  return HOT;
}

void GuiCmp::setTestRegion( const ShapeDef & shape ) {
  *testRegion = shape;
}
void GuiCmp::getTestRegion( ShapeDef * out ) const
{
  *out = *testRegion;
}

void GuiCmp::drawGui()
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    GuiInfo * guiInfo = records[i];

    if( guiInfo->widgetProcedure ) {
      guiInfo->widgetProcedure( guiInfo->entity, game, guiInfo );
    }
  }/*koniec for (i)*/

}

void GuiCmp::setVisibilityAll( bool v )
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    GuiInfo * guiInfo = records[i];
    ShapeDef * shapeDef = guiInfo->shapeDef;
    if( shapeDef )
      shapeDef->visible = v;
  }/*koniec for (i)*/
}

