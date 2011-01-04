//============================================================================
// Name        : empty_sdl.cpp
// Author      : boo
// Version     : 0.0v
// Copyright   : Free
// Description : początek pracy: 12.XI.2010
//============================================================================

#include "main.h"
#include "parser.h"

Entity nullEntity( 0 ) , gameEntity( 1 ), g_Player( 2 ), cursorId;

const float Game::TIME_STEP_S = ( 1.f / 60.f );  //sekundy
const uint32_t Game::TIME_STEP_MS = roundf( TIME_STEP_S * 1000.f ); //milisekundy

void playerCallback( Game * game, InputDef * inputDef, SDL_Event * event,
                     uint8_t * keyState )
{
  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * shapeDef =  shcmp->get( inputDef->entity );

  MovableCmp * mvcmp = game->getMovableCmp();
  MovableDef * movableDef = mvcmp->get( inputDef->entity );

  //jesli gra nie ma focusa
  if( game->getGuiCmp()->isFocused( gameEntity ) )
  {

    if( !keyState[SDLK_LCTRL] ) {

      if( keyState[SDLK_w] ) {
        movableDef->addAcc( Vec2( 0.f, -3200.f ) );
      }
      if( keyState[SDLK_s] ) {
        movableDef->addAcc( Vec2( 0.f, 3200.f ) );
      }
      if( keyState[SDLK_a] ) {
        movableDef->addAcc( Vec2( -3200.f, 0.f ) );
      }
      if( keyState[SDLK_d] ) {
        movableDef->addAcc( Vec2( 3200.f, 0.0f ) );
      }
    }
  }// if is focused
  if( game->getGuiCmp()->isActive(gameEntity))
  {
    const GuiAction& ga = game->getGuiCmp()->getActiveDetails();
    if(ga.hotInfo & GuiCmp::LEFT_RELEASED){
      int mouseX,mouseY;
      SDL_GetMouseState(&mouseX,&mouseY);
      const Vec2& worldPos = game->getRender()->scrToWorld(mouseX,mouseY);
      MapInfo * map = game->getMapCmp()->getSector(worldPos);
      if(map){
        if(game->editData.cursorTile >= 0)
          map->setTileTexture(1,game->editData.cursorTile,worldPos);
      }
    }
  }


  if( event ) {
    switch( event->type ) {

      case SDL_MOUSEMOTION:
        const XY<uint32_t>& wd = game->getRender()->getWindowDim();
        shapeDef->angle = std::atan2( event->motion.y - wd.y * 0.5f,
                                      event->motion.x - wd.x * 0.5f ) + PI * 0.5f;
        //printf("angle %f\n",shapeDef->angle * toDegreesFactor );

        ShapeDef * cursorShape = game->getShapeCmp()->get( cursorId );
        cursorShape->pos = Vec2( event->motion.x, event->motion.y );
        break;
    }

  }
  //shapeDef->angle = std::atan2(movableDef->v.y,movableDef->v.x);
}

Game::Game( int argc, char * argv[] ) :
  running( false ), mode( NORMAL )
  , render( new Render( this, XY<uint32_t>( 1024, 768 ), 24, false ) )
  , unit( 3 ), player( g_Player )
  , inputCmp( this )
  , spriteCmp( this ), textCmp( this ), shapeCmp( this ), movableCmp( this, 10 )
  , mapCmp( this )
  , guiCmp( this ), guiStyleCmp( this )
{
  unit[0] = nullEntity;
  unit[1] = gameEntity;
  unit[2] = player;

  parser.addObject( this, "game" );

  loadTextUnits( "a" );

  cursorId = createEntity();
  cursorId.saveIt = false;
  ShapeDef cursorShape;
  createRect( 32, 32, &cursorShape.rect );
  cursorShape.visible = false;
  cursorShape.depth = 0xffff;
  shapeCmp.add( cursorId, cursorShape );

  SpriteDef cursorDef;
  cursorDef.shape = cursorId;
  cursorDef.atlasFile = "img/a1.png";
  cursorDef.atlasInfoFile = "img/a1.tai";
  cursorDef.color = 0xff000000; //czerń
  cursorDef.coordSpace = Render::SCREEN_COORD;
  cursorDef.textureName = "blank.png";
  cursorDef.visible = false;    //narazie uzywam domyslnego
  spriteCmp.add( cursorId, cursorDef );


  running = true; //na koncu konstruktora
}

Game::~Game() {
  saveTextUnits( "a" );

  render->shutdownOGL();
  delete render;
}

int Game::run() {
  SDL_Event event;
  uint32_t delta = 0;
  uint32_t lastUpdate = SDL_GetTicks();
  uint32_t accumulator = 0;
  const uint32_t ACCUMULATED_MAX = 5 * TIME_STEP_MS ;
  uint8_t * keyState = SDL_GetKeyState( 0 );
  uint32_t frame = 0;

  while( running ) {
    delta = std::max( SDL_GetTicks() - lastUpdate, 0u );
    lastUpdate += delta;
    accumulator += delta;
    accumulator = clamp( accumulator, 0u, ACCUMULATED_MAX );

    while( SDL_PollEvent( &event ) )
    {
      //inputCmp.update();
      inputCmp.notifyEventAll( &event, keyState );

      //guiCmp.checkAll(inputCmp.getMousePos() );
      switch( event.type ) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_KEYDOWN:
          switch( event.key.keysym.sym ) {
            case SDLK_ESCAPE:
              running = false;
              break;
            case SDLK_F12:
              ++mode = mode % 2;
              break;
            case SDLK_s:
              if( keyState[SDLK_LCTRL] ) {
                saveTextUnits( "a" );
              }
              break;
            case SDLK_q:
              if( keyState[SDLK_LCTRL] ) {
                loadTextUnits( "a" );

                puts( "loaded from: save/a_{cmp}.txt" );
              }
              break;
            default:
              break;
          }// switch event.key.keysym.sym
          break;  //case SDL_KEYDOWN
        case SDL_MOUSEBUTTONDOWN:
          if( event.button.button == SDL_BUTTON_WHEELUP && keyState[SDLK_LCTRL] ) {
            render->zoomRational( 1.f / 512.f );
          }
          else if( event.button.button == SDL_BUTTON_WHEELDOWN
                   && keyState[SDLK_LCTRL] ) {
            render->zoomRational( -1.f / 512.f );
          }
          break;

        default:
          break;
      }
    }

    while( accumulator >= TIME_STEP_MS ) {
      accumulator -= TIME_STEP_MS;

      if( keyState[SDLK_EQUALS] && keyState[SDLK_LCTRL] ) {
        render->zoomLinear( 1.f / 1024.f );
      }
      else if( keyState[SDLK_MINUS] && keyState[SDLK_LCTRL] ) {
        render->zoomLinear( -1.f / 1024.f );
      }
      else if( keyState[SDLK_0] && keyState[SDLK_LCTRL] ) {
        render->setZoom( 16.f );
      }

      movableCmp.collectForces();

      //inputCmp.update();
      inputCmp.notifyEventAll( 0, keyState );

      movableCmp.update( TIME_STEP_S );

    }//koniec while (accumulator >= TIME_STEP_MS)
    movableCmp.clearForces();

    inputCmp.notifyFrameProgressAll( frame, InputCmp::BEGIN );

    ShapeDef * scd = shapeCmp.get( player );
    Vec2 worldPlayerCoord = scd->pos ;
    render->beginDraw( worldPlayerCoord );
    //render->beginDraw( RenderVec2() );

    //zapewnia grze focus
    guiCmp.initFrame( shapeCmp.getSure( gameEntity ) );

    guiCmp.drawGui();

    spriteCmp.drawAll();
    textCmp.drawAll();

    mapCmp.draw( scd->pos ); //podaje punkt w który spogląda kamera

    render->endDraw();
    //guiCmp.setVisibilityAll(false);

    //guiCmp.update();

    inputCmp.notifyFrameProgressAll( frame, InputCmp::END );
    inputCmp.onFrameEnd();

    ++frame;
  }//koniec while (running)

  return 0;
}

void Game::saveTextUnits( const char * name )
{
  //narazie to jest niebezpieczne
//  for(int i = 0; i < (int)unit.size(); ++i ){
//    resetEntity(&unit[i],Entity(i) );
//  }/*koniec for (i)*/

  char mask[] = "save/%s%s";
  char tmp[250];

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_input.txt" );
  printf( "saving to '%s'\n", tmp );
  inputCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_sprite.txt" );
  printf( "saving to '%s'\n", tmp );
  spriteCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_text.txt" );
  printf( "saving to '%s'\n", tmp );
  textCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_shape.txt" );
  printf( "saving to '%s'\n", tmp );
  shapeCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_movable.txt" );
  printf( "saving to '%s'\n", tmp );
  movableCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_map.txt" );
  printf( "saving to '%s'\n", tmp );
  mapCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_widget.txt" );
  printf( "saving to '%s'\n", tmp );
  guiCmp.saveText( tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_guistyle.txt" );
  printf( "saving to '%s'\n", tmp );
  guiStyleCmp.saveText( tmp );


  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_units.txt" );
  printf( "saving to '%s'\n", tmp );
  saveUnitsNumber( tmp );
}
void Game::loadTextUnits( const char * name )
{
  unit.clear();
  unit.push_back( nullEntity );
  unit.push_back( gameEntity );
  unit.push_back( g_Player );

  char mask[] = "save/%s%s";
  char tmp[250];

  inputCmp.clear();
  spriteCmp.clear();
  textCmp.clear();
  shapeCmp.clear();
  movableCmp.clear();
  mapCmp.clear();
  guiCmp.clear();
  guiStyleCmp.clear();

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_input.txt" );
  inputCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_sprite.txt" );
  spriteCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_text.txt" );
  textCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_shape.txt" );
  shapeCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_movable.txt" );
  movableCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_map.txt" );
  mapCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_widget.txt" );
  guiCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  memset( tmp, 0, sizeof( tmp ) );
  sprintf( tmp, mask, name, "_guistyle.txt" );
  guiStyleCmp.loadText( tmp, unit );
  printf( "loaded '%s'\n", tmp );

  std::sort( unit.begin(), unit.end() );
  std::vector<Entity>::iterator it = std::unique( unit.begin(), unit.end() );
  unit.resize( it - unit.begin() );

  inputCmp.afterLoad();
  spriteCmp.afterLoad();
  textCmp.afterLoad();
  shapeCmp.afterLoad();
  movableCmp.afterLoad();
  mapCmp.afterLoad();
  guiCmp.afterLoad();
  guiStyleCmp.afterLoad();


//  memset(tmp,0,sizeof(tmp) );
//  sprintf(tmp,mask,name,"_units.txt");
//  saveUnitsNumber(tmp);

  //nadpisuje wartość dla gameEntity - to co w pliku sie nie liczy
  const XY<uint32_t> scrS = render->getWindowDim();
  ShapeDef shapeDef;
  shapeDef.pos = RenderVec2( 0.f, 0.f );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( scrS.x, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, scrS.y );
  shapeDef.rect.downRight = Vec2( scrS.x, scrS.y );
  shapeDef.visible = false;
  shapeDef.depth = 32000;
  shapeCmp.overwrite( gameEntity, shapeDef );

  InputDef inputDef;
  inputDef.eventCallback = playerCallback;
  inputDef.eventCallbackName = "playerCallback";
  inputCmp.overwrite( player, inputDef );

  MovableDef * movable = movableCmp.get( player );
  if( !movable ) {
    MovableDef md;
    movableCmp.add( player, md );
  }
}

void Game::saveUnitsNumber( const char * filename )
{
  std::string str;
  char tmp[70];
  for( int i = 0; i < ( int )unit.size(); ++i ) {
    memset( tmp, 0, 70 );
    sprintf( tmp, "unit[%i] %i\n", i, unit[i].getId() );
    str += tmp;
  }/*koniec for (i)*/
  FILE * file = fopen( filename, "w" );
  fputs( str.c_str(), file );
  fclose( file );
}

////lepiej nie uzywać bo niedziała tak jak powinno
//void Game::resetEntity( Entity * dest, const Entity source )
//{
//  Entity & d = *dest;
//// TODO (boo#1#): dokonczyć: napisać dla każdego komponentu podobną funkcjie
//// która zamienia records[i]->entity na source
//  if( d == source )
//    return;
//
//  int destFound = -1;
//  int sourceFound = -1;
//  for( int i = 0; i < ( int )unit.size(); ++i ) {
//    if( unit[i] == d ) {
//      destFound = i;
//    }
//    else if( unit[i] == source ) {
//      sourceFound = i;
//    }
//  }/*koniec for (i)*/
//
//  if( destFound == -1 ) {
//    //nie znaleziono celu, kończymy
//    return;
//  }
////  if( sourceFound != -1 ){
////    //niby moznaby tu skończyć ale mozna też scalić jednostki
////  }
//
//  inputCmp.remapEntity( d, source );
//  spriteCmp.remapEntity( d, source );
//  textCmp.remapEntity( d, source );
//  shapeCmp.remapEntity( d, source );
//  movableCmp.remapEntity( d, source );
//  mapCmp.remapEntity( d, source );
//  guiCmp.remapEntity( d, source );
//
//  unit[destFound] = source;
//  *dest = source;
//}

int main( int argc, char * argv[] ) {
  Game game( argc, argv );
  game.run();

  return 0;
}

