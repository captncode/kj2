#include "guicmp.h"
#include "main.h"
#include "motioncmp.h"
#include "parser.h"
#include "widget.h"

const Entity GuiCmp::notFocused;

GuiState::GuiState(): hot( nullEntity ), active( nullEntity ), hotDepth( 0 )
  , kbdItem( gameEntity )
  , lastFocused( nullEntity ) , mouse( 0 ), keyEntered()
  , keyMod()
{
}

GuiState::~GuiState()
{
}



GuiInfo::GuiInfo() : state( false )
  , guiControl(), guiRespond()
  , depth( 0 ),barHeight(5),borderWidth(1),barSpriteEntity(0)
  ,borderSpriteEntity(0)
{
}

GuiInfo::GuiInfo( const GuiInfo & gi, Entity e, Game * ) : entity( e )
  , guiControl(gi.guiControl), guiRespond(gi.guiRespond)
  , guiControlName( gi.guiControlName ), guiRespondName( gi.guiRespondName )
  , state( gi.state ), depth( gi.depth ), barHeight(gi.barHeight)
  , borderWidth(gi.borderWidth), barSpriteEntity(gi.barSpriteEntity)
  , borderSpriteEntity(gi.borderSpriteEntity)
{
  const parserNs::BaseParserItem * bpi = parser.getFunction( guiControlName.c_str() );
  if( bpi ) {
    guiControl = bpi->getPointer<GuiControl_pf>();
    assert( guiControl );
  }
  bpi = parser.getFunction( guiRespondName.c_str() );
  if( bpi ) {
    guiRespond = bpi->getPointer<GuiRespond_pf>();
    assert( guiRespond );
  }

}

GuiInfo::GuiInfo( char * line [] ) : state( 0 ), depth( 0 ),
  guiControl(0), guiRespond(0),barHeight(0),borderWidth(0)
{
  int e = 0;
  sscanf( line[0], "%i", &e );
  entity = Entity( e );
  char tmp [250] = {};
  sscanf( line[1], "%250s", tmp );
  guiControlName = tmp;
  memset( tmp, 0, 250 );
  sscanf( line[2], "%250s", tmp );
  guiRespondName = tmp;

  sscanf( line[3], "%hi", &depth );
  sscanf( line[4], "%f", &barHeight );
  sscanf( line[5], "%f", &borderWidth);

  if(barHeight){
    e = 0;
    sscanf( line[6], "%i", &e );
    barSpriteEntity = Entity(e);
  }
  if(borderWidth){
    e = 0;
    sscanf( line[7], "%i", &e );
    borderSpriteEntity = Entity(e);
  }
}

GuiInfo::~GuiInfo()
{
}

std::string GuiInfo::getAsString()const {
  char tmp[250] = {};
  sprintf( tmp, "%i GuiInfo entity \n", entity.getId() );
  std::string out( tmp );
  memset( tmp, 0, 250 );

  if( guiControlName.length() > 0 ) {
    sprintf( tmp, " %s guiControlName \n", guiControlName.c_str() );
    out += tmp;
    memset( tmp, 0, 250 );
  }
  else {
    out += " 0 guiControlName \n";
  }
  if( guiRespondName.length() > 0 )
  {
    sprintf( tmp, " %s guiRespondName\n", guiRespondName.c_str() );
    out += tmp;
    memset( tmp, 0, 250 );
  }
  else {
    out += " 0 guirespondName \n";
  }

  sprintf( tmp, " %hi depth\n %f barHeight\n %f borderWidth\n", depth,barHeight,
          borderWidth);
  out += tmp;

  memset( tmp, 0, 250 );
  sprintf(tmp," %i barSpriteEntity\n %i borderSpriteEntity\n",
          barSpriteEntity.getId(),
          borderSpriteEntity.getId() );
  out += tmp;


  return out;
}

void GuiInfo::afterLoad(Game * game)
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
          guiCmp->state.mouse |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->state.mouse |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break;//SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      switch( event->button.button ) {
        case SDL_BUTTON_WHEELUP:
          guiCmp->state.mouse |= GuiCmp::WHEEL_UP;
          break;
        case SDL_BUTTON_WHEELDOWN:
          guiCmp->state.mouse |= GuiCmp::WHEEL_DOWN;
          break;
      }// switch( event.button.button )
      break; // SDL_MOUSEBUTTONUP:
    case SDL_KEYDOWN:
      if( event->key.state == SDL_PRESSED )
      {
        guiCmp->state.keyEntered = event->key.keysym.sym;
        guiCmp->state.keyMod |= event->key.keysym.mod;
        guiCmp->state.unicode = event->key.keysym.unicode;
      }//if(event->key->state == SDL_PRESSED)

      break; //case SDL_KEYDOWN:
    case SDL_KEYUP:
      if( event->key.state == SDL_RELEASED )
      {
        guiCmp->state.unicode = 0;
        guiCmp->state.keyMod &= ~event->key.keysym.mod;
        guiCmp->state.keyEntered = 0;
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
  , mark( 0 ), cursor( 0 )
{
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  e = game->createEntity();
  game->getInputCmp()->add( e, inputDef );

  parser.addFun2( &editX, "editX" );
  parser.addFun2( &consoleX, "consoleX" );
  parser.addFun2( &windowX, "windowX" );
  parser.addFun4( &windowXCallback,"windowXCallback" );
}
GuiCmp::~GuiCmp()
{
}

static inline void enableSDLEditMode() {
  SDL_EnableKeyRepeat( 5000, 300 );
  SDL_EnableUNICODE( 1 );
}
void GuiCmp::focusNext( GuiAction * ai )
{
  state.kbdItem = nullEntity;
  state.keyEntered = 0;
  ai->info &= ~FOCUSED;
  cursor = 0x7fff;
  mark = 0;
}

void GuiCmp::focusPrev( GuiAction * ai )
{
  state.kbdItem = state.lastFocused;
  state.keyEntered = 0;
  ai->info &= ~FOCUSED;
  cursor = 0x7fff;
  mark = 0;
}

GuiAction GuiCmp::actionTest( Entity e )
{
  GuiAction action;
  uint32_t & out = action.info;
  out = 0;
  if( nullEntity == state.kbdItem )
  {
    //przejmuje focus
    state.kbdItem = e;
  }

  if( e == state.kbdItem ) {
    out |= FOCUSED;
    enableSDLEditMode();

    switch( state.keyEntered )
    {
      case SDLK_TAB:
        if( SDLK_LSHIFT == state.keyMod )
        {
          focusPrev( &action );
          break;
        }
        focusNext( &action );
        break;

      case SDLK_RETURN:
        break;

    }//koniec switch'a
    action.key = state.keyEntered;
    action.unicode = state.unicode;
    action.keyMod = state.keyMod;
  }//koniec if'a

  state.lastFocused = e;

  XY<int32_t> mousePos;
  uint8_t mouseState = SDL_GetMouseState( &mousePos.x, &mousePos.y );
  ShapeDef * shapeDef = game->getShapeCmp()->get( e );
  GuiInfo * guiInfo = get( e );
  if( !shapeDef || !guiInfo)
    return GuiAction();

  //dzieki goto otrzymałem rozbudeoane if( ... && (...|| ... || ... ) )
  //kursor wewnatrz kontrolki
  if( shapeDef->isInside( mousePos ) ) {
    //zadna nie jest goraca wiec śmiało można klikać
    if( nullEntity == state.hot ) {
      //! moje pierwsze pierwsze  użycie goto
      goto makeHotLabel;
    }
//    if( prevState.hot == e)
//      goto makeHotLabel;
    if( state.hotDepth > guiInfo->depth ) {
      // glebokosc obecnie najechanej kontrolki jest mniejsza niz wcześniejszej
      // goracej wiec można śmiało ją zaznaczyć
      goto makeHotLabel;
    }
    if( !( e == state.hot ) )
    { //nie porownuje samego ze soba
      ShapeDef * hotShape = game->getShapeCmp()->get( state.hot );

      //jesli mysz nie jest w środku gorącego i jest w środku obecnego
      if( !hotShape->isInside( mousePos ) ) {
        goto makeHotLabel;
      }
    }
  }

  goto endLabel;

makeHotLabel:
//  if(makeHot)
  {
    uint8_t prevMouseState = game->getInputCmp()->getPrevFrameMouseState();

    state.hot = e;
    state.hotDepth = guiInfo->depth;
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
      state.active = e;
      state.kbdItem = e;
      enableSDLEditMode();

      cursor = 0x7fff;
      mark = 0;
    }
  }

endLabel:
  action.guiInfo = guiInfo;
  action.shapeDef = shapeDef;
  action.textInfo = 0;

  return action;

}

void GuiCmp::drawGui()
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    GuiAction action;
    GuiInfo * guiInfo = records[i];
    if( guiInfo->guiControl )
      action = guiInfo->guiControl( guiInfo->entity, game );
    if( guiInfo->guiRespond )
      guiInfo->guiRespond( &action, game );
  }/*koniec for (i)*/
}

void GuiCmp::update()
{
  prevState = state;
  state.mouse &= ~( WHEEL_DOWN | WHEEL_UP );

  if( SDLK_TAB == state.keyEntered ) {
    state.kbdItem = notFocused;
    SDL_EnableKeyRepeat( 0, 0 );
    SDL_EnableUNICODE( 0 );
  }

  state.keyEntered = 0;
  state.unicode = 0;

  state.hot = nullEntity;
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



