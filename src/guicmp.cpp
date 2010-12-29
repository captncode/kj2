#include "guicmp.h"
#include "main.h"
#include "motioncmp.h"

const Entity GuiCmp::notFocused;

GuiState::GuiState(): hot( nullEntity ), active( nullEntity ), hotDepth(0)
  ,kbdItem( Game::gameEntity ), lastFocused(nullEntity) ,mouse(0), keyEntered()
  ,keyMod()
{
}

GuiState::~GuiState()
{
}



GuiInfo::GuiInfo() : isMoving( false ), depth( 0 )
{
}
GuiInfo::GuiInfo( const GuiInfo & gi, Entity e, Game * ) : entity( e )
  , isMoving( gi.isMoving ), depth( gi.depth )
{
}
GuiInfo::GuiInfo( char* line []  ) : isMoving( 0 ), depth(0)
{
  sscanf(line[0],"%i",&entity );
  sscanf(line[1],"%hi",&depth );
}

GuiInfo::~GuiInfo()
{
}

std::string GuiInfo::getAsString()const {
  char tmp[512] = {};
  sprintf(tmp,"%i GuiInfo entity \n %hi depth",entity.getId(),depth );

  return std::string(tmp);
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

GuiCmp::GuiCmp( Game * game ) : BaseType( game )
  ,mark(0), cursor(0)
{
  InputDef inputDef;
  inputDef.eventCallback = guiCmpInputHandler;
  game->getInputCmp()->add( e, inputDef );
}
GuiCmp::~GuiCmp()
{
}

static inline void enableSDLEditMode(){
  SDL_EnableKeyRepeat(5000,300);
  SDL_EnableUNICODE( 1 );
}
GuiAction GuiCmp::actionTest( Entity e )
{
  GuiAction action;
  uint32_t & out = action.info;
  out = 0;
  if( nullEntity == state.kbdItem ) {
    state.kbdItem = e;
  }

  if( e == state.kbdItem ) {
    out |= FOCUSED;
    enableSDLEditMode();

    switch( state.keyEntered )
    {
      case SDLK_TAB:
        state.kbdItem = nullEntity;   //oddaje focus następnej kontrolce
        if( SDLK_LSHIFT == state.keyMod ) //albo poprzeniej
          state.kbdItem = state.lastFocused;
        state.keyEntered = 0;
        out &= ~FOCUSED;
        cursor = 0x7fff;
        mark = 0;
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
  GuiInfo * guiInfo = get(e);

  //dzieki goto otrzymałem rozbudeoane if( ... && (...|| ... || ... ) )
  //kursor wewnatrz kontrolki
  if(shapeDef->isInside( mousePos ) ){
    //zadna nie jest goraca wiec śmiało można klikać
    if( nullEntity == state.hot ){
      //! moje pierwsze pierwsze  użycie goto
      goto makeHotLabel;
    }
//    if( prevState.hot == e)
//      goto makeHotLabel;
    if( state.hotDepth > guiInfo->depth ){
      // glebokosc obecnie najechanej kontrolki jest mniejsza niz wcześniejszej
      // goracej wiec można śmiało ją zaznaczyć
      goto makeHotLabel;
    }
    if( !(e==state.hot) )
    { //nie porownuje samego ze soba
      ShapeDef * hotShape = game->getShapeCmp()->get( state.hot );

      //jesli mysz nie jest w środku gorącego i jest w środku obecnego
      if( !hotShape->isInside(mousePos) ){
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
    out |= isMouseButtonReleased( SDL_BUTTON_LEFT, prevMouseState,mouseState )
           * LEFT_RELEASED;
    out |= isMouseButtonReleased( SDL_BUTTON_MIDDLE, prevMouseState,mouseState )
           * MIDDLE_RELEASED;
    out |= isMouseButtonReleased( SDL_BUTTON_RIGHT, prevMouseState, mouseState )
           * RIGHT_RELEASED;

    out |= ( bool( SDL_BUTTON( SDL_BUTTON_LEFT ) & mouseState ) ) * LEFT_DOWN;
    out |= ( bool( SDL_BUTTON( SDL_BUTTON_MIDDLE ) & mouseState ))* MIDDLE_DOWN;
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

void GuiCmp::update()
{
  prevState = state;
  state.mouse &= ~( WHEEL_DOWN | WHEEL_UP );

  if(SDLK_TAB == state.keyEntered){
		state.kbdItem = notFocused;
		SDL_EnableKeyRepeat(0,0);
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


bool makeControlMovable(GuiAction& a, const Vec2& mouseDelta,
                        GuiCmp::ControlFlag whenStart)
{
  if( a )
  {
    if(a.info & whenStart){
      a.guiInfo->isMoving = true;
    }else{
      a.guiInfo->isMoving = false;
    }
  }
  if(a.guiInfo->isMoving ){
    a.shapeDef->pos += mouseDelta;
    if(a.textInfo){
      a.textInfo->position += mouseDelta;
      //po przemieszczeniu kursor poza kontrolką?
      XY<int32_t> mousePos;
      SDL_GetMouseState(&mousePos.x,&mousePos.y);
      if(! a.shapeDef->isInside(mousePos) )
        a.guiInfo->isMoving = false;
    }
  }
  return a.guiInfo->isMoving;
}
bool makeControlMovable(GuiAction& a, const XY<int32_t>& mouseDelta,
                        GuiCmp::ControlFlag whenStart)
{
  return makeControlMovable(a,Vec2(mouseDelta),whenStart );
}


bool makeControlResizable(GuiAction& a, const Vec2& mouseDelta,
                        GuiCmp::ControlFlag whenStart)
{
  if( a )
  {
    if(a.info & whenStart){
      a.guiInfo->isMoving = true;
    }else{
      a.guiInfo->isMoving = false;
    }
  }
  if(a.guiInfo->isMoving ){
    //a.shapeDef->pos += mouseDelta;
    a.shapeDef->rect.upRight += Vec2(mouseDelta.x,0.f);
    a.shapeDef->rect.downLeft += Vec2(0.f,mouseDelta.y);
    a.shapeDef->rect.downRight += Vec2(mouseDelta);

    if(a.textInfo){
      //a.textInfo->position += mouseDelta;
      //po przemieszczeniu kursor poza kontrolką?
      XY<int32_t> mousePos;
      SDL_GetMouseState(&mousePos.x,&mousePos.y);
      if(! a.shapeDef->isInside(mousePos) )
        a.guiInfo->isMoving = false;
    }
  }
  return a.guiInfo->isMoving;
}
bool makeControlResizable(GuiAction& a, const XY<int32_t>& mouseDelta,
                        GuiCmp::ControlFlag whenStart)
{
  return makeControlResizable(a,Vec2(mouseDelta),whenStart );
}

GuiAction labelX(const Entity e, Game * game)
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->get(e);

  //troche hujnia z tym, ale jest rozwiazanie, poprostu niech sobie render to
  //rysuje, a jak bede chcaił coś zmienić to zmienie i tak obiekt SpriteInfo
  //
  shapeDef->visible = true;
  spriteCmp->draw( e );
  shapeDef->visible = false;

  textInfo->position = shapeDef->pos;
  textInfo->visible = true;
  textCmp->draw(e);
  textInfo->visible = false;

  return action;
}

GuiAction consoleX(const Entity e, Game * game){

  ShapeCmp * shapeCmp = game->getShapeCmp();
  GuiCmp * guiCmp = game->getGuiCmp();

  GuiAction gameAction = guiCmp->actionTest(game->gameEntity);
  GuiAction a = guiCmp->actionTest(e);
  ShapeDef* &sh = a.shapeDef;
  const XY<int32_t> winDim = XY<int32_t>(game->getRender()->getWindowDim() );
  const int maxHeight = winDim.y * 0.5f;


  if( !a.shapeDef->visible )
  {// konsolka niewidoczna
    if(gameAction.key == SDLK_BACKQUOTE )   //pokazuje kontrolke
    {//wcisnieto ` i nalezy ja pokazac
      sh->pos = Vec2(0.f,0.f);
      sh->rect.upLeft = Vec2(0.f,0.f);
      sh->rect.upRight = Vec2(winDim.x,0.f);

      sh->rect.downLeft = Vec2(0.f,0.f);
      sh->rect.downRight = Vec2(winDim.x,0.f);

      a.textInfo = game->getTextCmp()->get(e);
      a.textInfo->position = Vec2(10.f,10.f);
      a.textInfo->color = makeARGB(255,200,10,10);
      sh->visible = true;    //rozwijaj
    }
    //toto niedziała narazie
    if(sh->rect.downLeft.y > 0)    //zwijam panie mieciu :P
    {
      sh->rect.downLeft.y -= 1;
      sh->rect.downRight.y -= 1;
    }
  }
  else{ //widzialna
    if(gameAction.key == SDLK_BACKQUOTE ) //pora sie schowac
    {
      sh->visible = false;
    }
    //rozwijanie
    if(sh->rect.downLeft.y < maxHeight)
    {
      sh->rect.downLeft.y += 1;
      sh->rect.downRight.y += 1;
    }
  }


  return a;
}

void drawGui(Game * game,int32_t mode)
{
  GuiAction a;
  const Entity *unit = &game->unit[0];
  GuiCmp * guiCmp = game->getGuiCmp();
  const Vec2 mouseDelta = Vec2(game->getInputCmp()->getMousePosDelta() );

  if( Game::NORMAL == mode )
  {
    if( a = editX( control[5], game ) )
    {
      if(a.key== SDLK_RETURN){
        puts("saving to:");
        std::string prefix = "save/";
        prefix += a.textInfo->text;

        std::string n1 = prefix;
        puts(n1.c_str() );

        n1 += "_shape.txt";
        game->getShapeCmp()->saveText( n1.c_str() );

        n1 = prefix;
        n1 += "_gui.txt";
        game->getGuiCmp()->saveText( n1.c_str() );

        n1 = prefix;
        n1 += "_sprite.txt";
        game->getSpriteCmp()->saveText( n1.c_str() );

        n1 = prefix;
        n1 += "_text.txt";
        game->getTextCmp()->saveText( n1.c_str() );
      }
    }
    makeControlMovable(a,mouseDelta,GuiCmp::RIGHT_DOWN);

    if( a = editX( control[6], game ) )
    {
      if(a.key== SDLK_RETURN){
        puts("load from:");
        std::string prefix = "save/";
        prefix += a.textInfo->text;

        std::string n1 = prefix;
        puts(n1.c_str() );

        n1 += "_shape.txt";
        game->getShapeCmp()->loadText( n1.c_str() );

        n1 = prefix;
        n1 += "_gui.txt";
        game->getGuiCmp()->loadText( n1.c_str() );

        n1 = prefix;
        n1 += "_sprite.txt";
        game->getSpriteCmp()->loadText( n1.c_str() );

        n1 = prefix;
        n1 += "_text.txt";
        game->getTextCmp()->loadText( n1.c_str() );
      }
    }
    makeControlMovable(a,mouseDelta,GuiCmp::RIGHT_DOWN);
  }
  else if( Game::EDIT == mode )
  {
    if(a = controlPaneX(control[4],game) ){

    }
    bool paneXMoving =false;
    if( !makeControlResizable(a,mouseDelta,GuiCmp::LEFT_DOWN) )
    {
      paneXMoving = makeControlMovable(a,mouseDelta,GuiCmp::RIGHT_DOWN);
    }


    a = editX(control[2],game);
    makeControlMovable(a,mouseDelta,GuiCmp::RIGHT_DOWN);

    if(paneXMoving){
      a.shapeDef->pos += mouseDelta;
    }

  }
  //byle który mode :P

  GuiAction consoleA;
  if(consoleA = consoleX(control[7],game ) )
  {
    a = guiCmp->actionTest(control[8]);
    Vec2Quad& consR = consoleA.shapeDef->rect;
    ShapeDef& editS = *a.shapeDef;

    const int editH = 10.f;
    editS.rect.upLeft = consR.downLeft - Vec2(0.f,editH);
    editS.rect.upRight = consR.downRight - Vec2(0.f,editH);
    editS.rect.downLeft = consR.downLeft;
    editS.rect.downRight = consR.downRight;

    if( a = editX(control[8],game) ){
      if(a.key == SDLK_RETURN){
        consoleA.textInfo->text += '\n';
        consoleA.textInfo->text += a.textInfo->text;
        puts(a.textInfo->text.c_str() );
        a.textInfo->text.clear();
      }
    }

  }

}

static void eraseMarkedTekst(std::string & str,int16_t & cursor,int16_t & mark)
{
  int16_t mark2 = mark;
  int16_t cursor2 = cursor;
  if(mark<0){
    mark2 = -mark2;
    cursor2 = cursor2 + mark;
    cursor += mark;
  }
  str.erase(cursor2,mark2);

  mark = 0;
}

GuiAction editX(const Entity e , Game * game )
{
  GuiCmp * guiCmp = game->getGuiCmp();
  TextCmp * textCmp = game->getTextCmp();

  GuiAction action = guiCmp->actionTest( e );

  SpriteCmp * spriteCmp = game->getSpriteCmp();
  ShapeDef * &shapeDef = action.shapeDef;
  TextInfo * &textInfo = action.textInfo;
  textInfo = textCmp->get(e);

  shapeDef->visible = true;
  spriteCmp->draw( e );
  shapeDef->visible = false;

  std::string & str = textInfo->text;

  textInfo->position = shapeDef->pos;
  textInfo->visible = true;
  textCmp->draw(e);
  textInfo->visible = false;

  if(action.info & GuiCmp::FOCUSED){

    int16_t & cursor = guiCmp->cursor;
    int16_t & mark = guiCmp->mark;


    cursor = std::min(int16_t( str.size() ),cursor);

    switch( action.key )
    {
      case SDLK_BACKSPACE:
        if(mark != 0){
          eraseMarkedTekst(str,cursor,mark);
        }
        else if( guiCmp->cursor > 0 ) {
          str.erase( guiCmp->cursor - 1, 1 );
          --guiCmp->cursor;
        }
        break;
      case SDLK_DELETE:
        if(mark != 0){
          eraseMarkedTekst(str,cursor,mark);
        }
        if( guiCmp->cursor < str.size() ) {
          str.erase( guiCmp->cursor, 1 );
        }
        break;

      case SDLK_LEFT:
        cursor = std::max( int16_t( 0 ), --guiCmp->cursor );
        if( action.keyMod & KMOD_CTRL ) {
          uint32_t found =
            str.find_last_of( ",./;\'[]\\=-~!@#$%^&*()_+ <>?:\"{}|", cursor-1 );
            //str.find_last_not_of( "qwertyuiopasdfghjklzxcvbnm", cursor );

          if( found != std::string::npos )
          {//nie znaleziono
            mark = cursor+1;
            cursor = found;
          }
          else{
            mark = cursor+1;
            cursor = 0;
          }
          //nie trzeba zaznaczać, bo brak shifta
          if(!(action.keyMod & KMOD_SHIFT) ){
            mark = 0;
          }

        }else if(action.keyMod & KMOD_SHIFT){
          ++mark;
        }
        else{

          mark = 0;
        }
        break;
      case SDLK_RIGHT:
        cursor = std::min( int16_t(str.size() ), ++guiCmp->cursor );
        if( action.keyMod & KMOD_SHIFT)
        {
          //kursor oddala sie od zaznaczenia
          --mark;
        }else{

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
        if(mark != 0){
          eraseMarkedTekst(str,cursor,mark);
        }
        str.insert( guiCmp->cursor++, 1, action.unicode );

        //str += action.key;
        break;
    }//switch(action.key)

    //rysowanie kursora
    int width = textCmp->getTextWidth(str.c_str(),textInfo->font,cursor);
    int height = textCmp->getFontHeight(textInfo->font);
    const int cursorWidth = 2;

    ShapeDef def;
    def.pos = textInfo->position + Vec2(width,0.f);
    def.visible = true;
    def.rect.upLeft = RenderVec2(0.f,0.f);
    def.rect.upRight = RenderVec2(cursorWidth,0.f);
    def.rect.downLeft = RenderVec2(0.f,height);
    def.rect.downRight = RenderVec2(cursorWidth,height);

    spriteCmp->drawRect(def,textInfo->color,textInfo->depth);

    if(mark != 0)
    { //cos zaznaczono
      int mark2 = cursor + mark;
      int cursor2 = cursor;
      if(mark < 0){ //zamiana pozycji
        mark2 = cursor;
        cursor2 = cursor + mark;
      }

      int markEndWidth = textCmp->getTextWidth(str.c_str(),textInfo->font,
                                                 mark2);
      int markBeginWidth = textCmp->getTextWidth(str.c_str(),textInfo->font,
                                                 cursor2);

      def.pos = textInfo->position + Vec2(markBeginWidth,0.f);
      const int markWidth = markEndWidth - markBeginWidth;
      def.visible = true;
      def.rect.upLeft = RenderVec2(0.f,0.f);
      def.rect.upRight = RenderVec2(markWidth,0.f);
      def.rect.downLeft = RenderVec2(0.f,height);
      def.rect.downRight = RenderVec2(markWidth,height);

      spriteCmp->drawRect(def,makeARGB(150,220,30,30),textInfo->depth);

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
  textInfo = textCmp->get(e);

  shapeDef->visible = true;
  spriteCmp->draw( e );
  shapeDef->visible = false;

  return action;

}


