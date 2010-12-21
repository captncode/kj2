//============================================================================
// Name        : empty_sdl.cpp
// Author      : boo
// Version     : 0.0v
// Copyright   : Free
// Description : początek pracy: 12.XI.2010
//============================================================================

#include "main.h"

const Entity Game::gameEntity;
Entity nullEntity;

const float Game::TIME_STEP_S = ( 1.f / 60.f );  //sekundy
const uint32_t Game::TIME_STEP_MS = roundf( TIME_STEP_S * 1000.f ); //milisekundy

void playerCallback( Game * game, InputDef * inputDef, SDL_Event * event,
                     uint8_t * keyState )
{
  //jesli gra nie ma focusa
  if( !game->getGuiCmp()->isFocused(game->gameEntity) )
    return;

  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * shcmpdata =  shcmp->get( inputDef->entity );

  MovableCmp * mvcmp = game->getMovableCmp();
  MovableDef * movableDef = mvcmp->get( inputDef->entity );

  if( keyState[SDLK_w] ) {
    movableDef->addAcc( Vec2( 0.f, -50.f ) );
  }
  if( keyState[SDLK_s] ) {
    movableDef->addAcc( Vec2( 0.f, 50.f ) );
  }
  if( keyState[SDLK_a] ) {
    movableDef->addAcc( Vec2( -50.f, 0.f ) );
  }
  if( keyState[SDLK_d] ) {
    movableDef->addAcc( Vec2( 50.f, 0.0f ) );
  }

}

Game::Game( int argc, char * argv[] ) :
  running( false ), mode( NORMAL )
  , render( new Render( this, XY<uint32_t>( 800, 600 ), 24, false ) ), inputCmp( this )
  , spriteCmp( this ), textCmp( this ), shapeCmp( this ), movableCmp( this, 10 )
  , mapCmp( this ), guiCmp( this )
{
  SpriteDef spriteDef;


  InputDef inputDef;
  inputDef.eventCallback = playerCallback;
  inputCmp.add( player, inputDef );

  spriteDef.atlasFile = "img/by_ftorek.png";
  spriteDef.atlasInfoFile = "img/by_ftorek.tai";
  spriteDef.textureName = "creature_human_blue.png";
  spriteDef.coordSpace = Render::WORLD_COORD;
  spriteDef.color = makeARGB( 255, 255, 255, 255 );
  spriteCmp.add( player, spriteDef );

  ShapeDef shapeDef;
  shapeDef.pos = RenderVec2( 0.f, 0.f );
  shapeDef.rect.upLeft = Vec2( 1.f, 1.f );
  shapeDef.rect.upRight = Vec2( -1.f, 1.f );
  shapeDef.rect.downLeft = Vec2( 1.f, -1.f );
  shapeDef.rect.downRight = Vec2( -1.f, -1.f );


  shapeCmp.overwrite( player, shapeDef );

  MovableDef mcd;
  mcd.m = 80.1f;
  mcd.frictionFactor = 0.98f;
  mcd.maxVel = 20.f;
  movableCmp.add( player, mcd );

  //pierwszy wrog
  spriteDef.textureName = "creature_monster_skeleton.png";
  spriteDef.coordSpace = Render::WORLD_COORD;
  spriteDef.color = makeARGB( 255, 255, 255, 255 );
  spriteCmp.add( enemy[0], spriteDef );

  shapeDef.pos = RenderVec2( 20.f, 30.f );
  shapeCmp.overwrite( enemy[0], shapeDef );

  mcd.m = 100.1f;
  movableCmp.add( enemy[0], mcd );


  MapDef mapDef;
  mapDef.filename = "map/level1.map";
  mapCmp.add( map[0], mapDef );

  mapDef.filename = "map/level2.map";
  mapCmp.add( map[1], mapDef );

  TextInfo textInfo;
  textInfo.text = "dupa dupa cycki\nafafasf\tasdasds";
  textInfo.font = TextCmp::consolasFont;
  textInfo.position = RenderVec2( 20.f, 20.f );
  textInfo.color = makeARGB( 190, 180, 0, 255 );
  textInfo.coordSpace = Render::SCREEN_COORD;

  textCmp.add( text[0], textInfo );

  textInfo.text = "zmienna: 1";
  textInfo.position = RenderVec2( 50.f, 50.f );
  textInfo.color = makeARGB( 190, 180, 0, 255 );
  textInfo.coordSpace = Render::SCREEN_COORD;
  textCmp.add( text[1], textInfo );


  spriteDef.atlasFile = "img/a1.png";
  spriteDef.atlasInfoFile = "img/a1.tai";
  spriteDef.textureName = "blank.png";
  spriteDef.coordSpace = Render::SCREEN_COORD;
  spriteDef.color = makeARGB( 255, 255, 255, 255 );

  spriteCmp.add( control[0], spriteDef );
  spriteCmp.add( control[1], spriteDef );
  spriteCmp.add( control[2], spriteDef );
  spriteCmp.add( control[3], spriteDef );
  spriteCmp.add( control[4], spriteDef );
  spriteCmp.add( control[5], spriteDef );
  spriteCmp.add( control[6], spriteDef );

  shapeDef.pos = RenderVec2( 500, 100 );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( 50.f, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, 50.f );
  shapeDef.rect.downRight = Vec2( 50.f, 50.f );
  //shapeDef.depth = 0;
  shapeCmp.overwrite( control[0], shapeDef );

  shapeDef.pos = RenderVec2( 700, 100 );
  shapeCmp.overwrite( control[1], shapeDef );

  shapeDef.pos = RenderVec2( 200, 500 );
  shapeCmp.overwrite( control[2], shapeDef );


  shapeDef.pos = RenderVec2( 500, 300 );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( 50.f, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, 50.f );
  shapeDef.rect.downRight = Vec2( 50.f, 50.f );

  shapeDef.pos = RenderVec2( 500, 500 );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( 50.f, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, 50.f );
  shapeDef.rect.downRight = Vec2( 50.f, 50.f );
  shapeDef.visible = false;

  shapeCmp.overwrite( control[4], shapeDef );

  shapeDef.pos = RenderVec2( 400, 500 );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( 50.f, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, 50.f );
  shapeDef.rect.downRight = Vec2( 50.f, 50.f );
  shapeDef.visible = false;

  shapeCmp.overwrite( control[5], shapeDef );

  shapeDef.pos = RenderVec2( 400, 400 );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( 50.f, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, 50.f );
  shapeDef.rect.downRight = Vec2( 50.f, 50.f );
  shapeDef.visible = false;

  shapeCmp.overwrite( control[6], shapeDef );

  const XY<uint32_t> scrS = render->getWindowDim();

  shapeDef.pos = RenderVec2( 0.f, 0.f );
  shapeDef.rect.upLeft = Vec2( 0.f, 0.f );
  shapeDef.rect.upRight = Vec2( scrS.x, 0.f );
  shapeDef.rect.downLeft = Vec2( 0.f, scrS.y );
  shapeDef.rect.downRight = Vec2( scrS.x, scrS.y );
  shapeDef.visible = false;
  shapeDef.depth = 32000;
  shapeCmp.overwrite( gameEntity, shapeDef );

  guiCmp.get(gameEntity)->depth = 32000;
  running = true; //na koncu konstruktora
}

Game::~Game() {
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
            default:
              break;
          }// switch event.key.keysym.sym
          break;  //case SDL_KEYDOWN
        case SDL_MOUSEBUTTONDOWN:
          if( event.button.button == SDL_BUTTON_WHEELUP && keyState[SDLK_LCTRL] ) {
            render->zoomRational( 1.f / 1.f );
          }
          else if( event.button.button == SDL_BUTTON_WHEELDOWN
                   && keyState[SDLK_LCTRL] ) {
            render->zoomRational( -1.f / 1.f );
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
      }else if( keyState[SDLK_MINUS] && keyState[SDLK_LCTRL] ) {
        render->zoomLinear( -1.f / 1024.f );
      }else if( keyState[SDLK_0] && keyState[SDLK_LCTRL] ) {
        render->setZoom( 16.f );
      }

      movableCmp.collectForces();

      //inputCmp.update();
      inputCmp.notifyEventAll( 0, keyState );

      movableCmp.update( TIME_STEP_S );

    }//koniec while (accumulator >= TIME_STEP_MS)
    movableCmp.clearForces();

    inputCmp.notifyFrameProgressAll(frame,InputCmp::BEGIN);

    ShapeDef * scd = shapeCmp.get( player );
    render->beginDraw( scd->pos );
    //render->beginDraw( RenderVec2() );

    //zapewnia grze focus
    guiCmp.actionTest(gameEntity);
    drawGui(this,mode );

    spriteCmp.drawAll();
    textCmp.drawAll();

    mapCmp.draw( scd->pos ); //podaje punkt w który spogląda kamera

    render->endDraw();
    //guiCmp.setVisibilityAll(false);

    guiCmp.update();

    inputCmp.notifyFrameProgressAll(frame,InputCmp::END);
    inputCmp.onFrameEnd();

    ++frame;
  }//koniec while (running)

  return 0;
}

int main( int argc, char * argv[] ) {
  Game game( argc, argv );
  game.run();

  return 0;
}

