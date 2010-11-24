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

void playerCallback(Game *game, InputCmpData * icd,SDL_Event * event,uint8_t * keyState)
{
  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeCmpData * shcmpdata =  shcmp->get( icd->entity );

  MovableCmp * mvcmp = game->getMovableCmp();
  MovableCmpData * mvcmpdata = mvcmp->get( icd->entity );

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
    mvcmpdata->addAcc(Vec2(0.f,-500.f) );
  }
  if(keyState[SDLK_s]){
    mvcmpdata->addAcc(Vec2(0.f,500.f) );
  }
  if(keyState[SDLK_a]){
    mvcmpdata->addAcc(Vec2(-500.f,0.f) );
  }
  if(keyState[SDLK_d]){
    mvcmpdata->addAcc(Vec2(500.f,0.0f) );
  }

}


Game::Game( int argc, char* argv[] ) : inputCmp(this),spriteCmp(this),
  shapeCmp(this), movableCmp(this,10)
{
	running = false;

	render = new Render(this);
  render->initOGL(XY<uint32_t>(800,600),24,false);


  InputCmpData icd;
  icd.callback = playerCallback;
  inputCmp.add(player, icd);

  SpriteCmpDef rcd;
  rcd.atlasFile = "img\\a1.png";
  rcd.atlasInfoFile = "img\\a1.tai";
  rcd.textureName = "hero0.png";
  rcd.coordSpace = Render::WORLD_COORD;
  rcd.color = makeRGBA(255,255,255,255);
  spriteCmp.add(player,rcd);

  ShapeCmpData scd;
  scd.pos = RenderVec2(0.f,0.f);
  scd.shape.upLeft = Vec2(10.f,10.f);
  scd.shape.upRight = Vec2(100.f,10.f);
  scd.shape.downLeft = Vec2(10.f,100.f);
  scd.shape.downRight = Vec2(100.f,100.f);
  shapeCmp.overwrite(player,scd);

  MovableCmpData mcd;
  mcd.m = 0.1f;
  movableCmp.add(player,mcd);

  //pierwszy wrog
  rcd.textureName = "enemy0.png";
  rcd.coordSpace = Render::WORLD_COORD;
  rcd.color = makeRGBA(255,255,255,255);
  spriteCmp.add(enemy[0],rcd);

  scd.pos = RenderVec2(50.f,50.f);
  shapeCmp.overwrite(enemy[0],scd);

  mcd.m = 100.1f;
  movableCmp.add(enemy[0],mcd);



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
            render->zoomRational(1.f/128.f);
          }else if (event.button.button == SDL_BUTTON_WHEELDOWN
                    && keyState[SDLK_LCTRL] ){
            render->zoomRational(-1.f/128.f);
          }
				break;

				default:
				break;
			}
		}

		while (accumulator >= TIME_STEP_MS){
			accumulator -= TIME_STEP_MS;

			inputCmp.handleEvent(0,keyState);
			movableCmp.update(TIME_STEP_S);

		}//koniec while (accumulator >= TIME_STEP_MS)
		movableCmp.clearForces();

    ShapeCmpData* scd = shapeCmp.get(player);
    render->beginDraw( scd->pos );
    //render->beginDraw( RenderVec2() );

    spriteCmp.draw();
    render->endDraw();

	}//koniec while (running)

	return 0;
}

int main( int argc, char* argv[] ){
	Game game(argc, argv);
	game.run();

	return 0;
}

