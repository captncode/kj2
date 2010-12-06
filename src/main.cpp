//============================================================================
// Name        : empty_sdl.cpp
// Author      : boo
// Version     : 0.0v
// Copyright   : Free
// Description : początek pracy: 12.XI.2010
//============================================================================

#include "main.h"

const float Game::TIME_STEP_S = (1.f/60.f);      //sekundy
const uint32_t Game::TIME_STEP_MS = roundf( TIME_STEP_S * 1000.f ); //milisekundy

void playerCallback(Game *game, InputDef * icd,SDL_Event * event,uint8_t * keyState)
{
  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * shcmpdata =  shcmp->get( icd->entity );

  MovableCmp * mvcmp = game->getMovableCmp();
  MovableDef * movableDef = mvcmp->get( icd->entity );

  if(event){
    switch(event->type)
    {
      case SDL_KEYDOWN:
        switch(event->key.keysym.sym)
        {
          default:
          break;
        }// koniec switch
      break; // case SDL_KEYDOWN

      default:
        break;
    }// koniec switch
  }//koniec if(event)

  if(keyState[SDLK_w]){
    movableDef->addAcc(Vec2(0.f,-50.f) );
  }
  if(keyState[SDLK_s]){
    movableDef->addAcc(Vec2(0.f,50.f) );
  }
  if(keyState[SDLK_a]){
    movableDef->addAcc(Vec2(-50.f,0.f) );
  }
  if(keyState[SDLK_d]){
    movableDef->addAcc(Vec2(50.f,0.0f) );
  }

}


Game::Game( int argc, char* argv[] ) :
  render(new Render(this,XY<uint32_t>(800,600),24,false) ), inputCmp(this),
  spriteCmp(this), shapeCmp(this), movableCmp(this,10),mapCmp(this)
{
	running = false;


  InputDef icd;
  icd.callback = playerCallback;
  inputCmp.add(player, icd);

  SpriteDef rcd;
  rcd.atlasFile = "img/a1.png";
  rcd.atlasInfoFile = "img/a1.tai";
  rcd.textureName = "hero0.png";
  rcd.coordSpace = Render::WORLD_COORD;
  rcd.color = makeRGBA(255,255,255,255);
  spriteCmp.add(player,rcd);

  ShapeDef scd;
  scd.pos = RenderVec2(0.f,0.f);
  scd.shape.upLeft = Vec2(-1.f,-1.f);
  scd.shape.upRight = Vec2(1.f,-1.f);
  scd.shape.downLeft = Vec2(-1.f,1.f);
  scd.shape.downRight = Vec2(1.f,1.f);
  shapeCmp.overwrite(player,scd);

  MovableDef mcd;
  mcd.m = 80.1f;
  mcd.frictionFactor = 1.0f;
  mcd.maxVel = 20.f;
  movableCmp.add(player,mcd);

  //pierwszy wrog
  rcd.textureName = "enemy0.png";
  rcd.coordSpace = Render::WORLD_COORD;
  rcd.color = makeRGBA(255,255,255,255);
  spriteCmp.add(enemy[0],rcd);

  scd.pos = RenderVec2(20.f,30.f);
  shapeCmp.overwrite(enemy[0],scd);

  mcd.m = 100.1f;
  movableCmp.add(enemy[0],mcd);


  MapDef mapDef;
  mapDef.filename = "map/level1.map";
  mapCmp.add(map[0],mapDef);

  mapDef.filename = "map/level2.map";
  mapCmp.add(map[1],mapDef);

	running = true; //na koncu konstruktora
}

Game::~Game(){
  render->shutdownOGL();
  delete render;
}

int Game::run(){
	SDL_Event event;
	uint32_t delta = 0;
	uint32_t lastUpdate = SDL_GetTicks();
	uint32_t accumulator = 0;
	const uint32_t ACCUMULATED_MAX = 5 * TIME_STEP_MS ;
	uint8_t * keyState = SDL_GetKeyState( 0 );

	while (running){
		delta = std::max(SDL_GetTicks() - lastUpdate,0u );
		lastUpdate += delta;
		accumulator += delta;
		accumulator = clamp( accumulator, 0u, ACCUMULATED_MAX );

		while (SDL_PollEvent(&event))
		{
		  inputCmp.handleEvent(&event,keyState);
			switch (event.type) {
				case SDL_QUIT:
					running = false;
				break;
				case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_ESCAPE)
            running = false;
				break;
				case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == SDL_BUTTON_WHEELUP&&keyState[SDLK_LCTRL] ){
            render->zoomRational(1.f/1.f);
          }else if (event.button.button == SDL_BUTTON_WHEELDOWN
                    && keyState[SDLK_LCTRL] ){
            render->zoomRational(-1.f/1.f);
          }
				break;

				default:
				break;
			}
		}



		while (accumulator >= TIME_STEP_MS){
			accumulator -= TIME_STEP_MS;
      if(keyState[SDLK_EQUALS] && keyState[SDLK_LCTRL] ){
        render->zoomRational(1.f/1.f);
      }else if(keyState[SDLK_MINUS] && keyState[SDLK_LCTRL] ){
        render->zoomRational(-1.f/1.f);
      }

      movableCmp.collectForces();
			inputCmp.handleEvent(0,keyState);
			movableCmp.update(TIME_STEP_S);

		}//koniec while (accumulator >= TIME_STEP_MS)
		movableCmp.clearForces();

    ShapeDef* scd = shapeCmp.get(player);
    render->beginDraw( scd->pos );
    //render->beginDraw( RenderVec2() );

    render->printText("cycki\n",Vec2(10.f,10.f));
    render->printText("dupa");

    spriteCmp.draw();
    mapCmp.draw(scd->pos);   //podaje punkt w który spogląda kamera

    render->endDraw();

	}//koniec while (running)

	return 0;
}

int main( int argc, char* argv[] ){
	Game game(argc, argv);
	game.run();

	return 0;
}

