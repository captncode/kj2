//============================================================================
// Name        : empty_sdl.cpp
// Author      : boo
// Version     : 0.0v
// Copyright   : Free
// Description : początek pracy: 12.XI.2010
//============================================================================

#include "main.h"

Entity nullEntity;

const float Game::TIME_STEP_S = (1.f/60.f);      //sekundy
const uint32_t Game::TIME_STEP_MS = roundf( TIME_STEP_S * 1000.f ); //milisekundy

void incAndShow(Game * game )
{
  TextCmp * textCmp = game->getTextCmp();
  TextInfo * textInfo = textCmp->get(game->text[1] );
  int a = 0;
  char tmpStr[textInfo->text.size()] ;
  memset(tmpStr,0,textInfo->text.size() );
  sscanf(textInfo->text.c_str(),"%s %i",tmpStr,&a);
  ++a;

  char strA[50] = {};
  sprintf(strA," %i",a);
  textInfo->text = tmpStr;
  textInfo->text += strA;
}

void decAndShow(Game * game )
{

}

uint32_t button1(Entity e , Game * game,int * a)
{
  GuiCmp * guiCmp = game->getGuiCmp();

  TextCmp *textCmp = game->getTextCmp();
  TextInfo * textInfo = textCmp->get(e);

  ShapeDef * shapeDef = game->getShapeCmp()->get( e );

  char buffer[30] = {};
  sprintf(buffer,"%i",*a);

  if(textInfo)
  {
    textInfo->text = buffer;
  }
  else{
    TextInfo ti;
    ti.text = buffer;
    ti.position = shapeDef->pos + RenderVec2(-5.f,-5.f);
    ti.color = makeARGB(190,180,0,255);
    ti.depth = shapeDef->depth +1;

    textCmp->add(e,ti);
  }
  return guiCmp->check(e);
}

void button1Callback(Game *game, InputDef * inputDef,SDL_Event * event,
                     uint8_t * keyState)
{
  Entity & e = inputDef->entity;
  GuiInfo * gi = game->getGuiCmp()->get(e);
  bool & moving = gi->isMoving;

  if(event) //tak naprawte ten if jest zbedny
  {
    // treraz wystarczy przenieśc moving do guiInfo i bedzie grało :)
    //static bool  moving = false;
    //done :)

    ShapeDef * shapeDef = game->getShapeCmp()->get(e);
    Vec2 tmpPos;
    uint8_t buttonState = SDL_GetMouseState( 0, 0 );
    XY<int32_t> mousePos = game->getInputCmp()->getMousePos();
    // podczas sprawdzania czy mysz jest nad oknem dzieki temu odejmowaniu
    // nie musze dodawać do aabb kontrolki jej pozycji
    // poprostu 'przesuwam' kontrolke do poczatku układu współrzednych
    // i kursor przesuwam o taki sam wektor
    tmpPos = mousePos - shapeDef->pos;

    XY<int32_t> relPos = game->getInputCmp()->getMousePosDelta();

    if( shapeDef->rect.isInside( tmpPos) )
    { // kursor wsrodku kontrolki
      game->getSpriteCmp()->setColor(e,makeARGB(0xff,0xf0,0xe0,0xe0) );

      if( buttonState & SDL_BUTTON(SDL_BUTTON_LEFT) )
      { // lewy przycisk wciśniety
        moving = true;
      }else {
        moving = false;
      }
    }else{    //rect.isInside
      game->getSpriteCmp()->setColor(e,makeARGB(0x150,0xff,0xff,0xff) );
    }
    if(moving)
    { //kursor napewno w srodku kontrolki
      if( buttonState & SDL_BUTTON(SDL_BUTTON_LEFT) ){
        shapeDef->pos += relPos;
      }else{
        moving = false;
      }
    }//if moving

  }//if event
}

void playerCallback(Game *game, InputDef * inputDef,SDL_Event * event,uint8_t * keyState)
{
  ShapeCmp * shcmp = game->getShapeCmp();
  ShapeDef * shcmpdata =  shcmp->get( inputDef->entity );

  MovableCmp * mvcmp = game->getMovableCmp();
  MovableDef * movableDef = mvcmp->get( inputDef->entity );

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
  render(new Render(this,XY<uint32_t>(800,600),24,false) ), inputCmp(this)
  ,spriteCmp(this), textCmp(this), shapeCmp(this), movableCmp(this,10)
  ,mapCmp(this),guiCmp(this)
{
	running = false;


  InputDef inputDef;
  inputDef.callback = playerCallback;
  inputCmp.add(player, inputDef);

  SpriteDef spriteDef;
  spriteDef.atlasFile = "img/by_ftorek.png";
  spriteDef.atlasInfoFile = "img/by_ftorek.tai";
  spriteDef.textureName = "creature_human_blue.png";
  spriteDef.coordSpace = Render::WORLD_COORD;
  spriteDef.color = makeARGB(255,255,255,255);
  spriteCmp.add(player,spriteDef);

  ShapeDef shapeDef;
  shapeDef.pos = RenderVec2(0.f,0.f);
  shapeDef.rect.upLeft = Vec2(1.f,1.f);
  shapeDef.rect.upRight = Vec2(-1.f,1.f);
  shapeDef.rect.downLeft = Vec2(1.f,-1.f);
  shapeDef.rect.downRight = Vec2(-1.f,-1.f);


  shapeCmp.overwrite(player,shapeDef);

  MovableDef mcd;
  mcd.m = 80.1f;
  mcd.frictionFactor = 0.98f;
  mcd.maxVel = 20.f;
  movableCmp.add(player,mcd);

  //pierwszy wrog
  spriteDef.textureName = "creature_monster_skeleton.png";
  spriteDef.coordSpace = Render::WORLD_COORD;
  spriteDef.color = makeARGB(255,255,255,255);
  spriteCmp.add(enemy[0],spriteDef);

  shapeDef.pos = RenderVec2(20.f,30.f);
  shapeCmp.overwrite(enemy[0],shapeDef);

  mcd.m = 100.1f;
  movableCmp.add(enemy[0],mcd);


  MapDef mapDef;
  mapDef.filename = "map/level1.map";
  mapCmp.add(map[0],mapDef);

  mapDef.filename = "map/level2.map";
  mapCmp.add(map[1],mapDef);

  TextInfo textInfo;
  textInfo.text = "dupa dupa cycki\nafafasf\tasdasds";
  textInfo.font = TextCmp::consolasFont;
  textInfo.position = RenderVec2(20.f,20.f);
  textInfo.color = makeARGB(190,180,0,255);
  textInfo.coordSpace = Render::SCREEN_COORD;

  textCmp.add(text[0], textInfo );

  textInfo.text = "zmienna: 1";
  textInfo.position = RenderVec2(50.f,50.f);
  textInfo.color = makeARGB(190,180,0,255);
  textInfo.coordSpace = Render::SCREEN_COORD;
  textCmp.add(text[1], textInfo );


  spriteDef.atlasFile = "img/a1.png";
  spriteDef.atlasInfoFile = "img/a1.tai";
  spriteDef.textureName = "blank.png";
  spriteDef.coordSpace = Render::SCREEN_COORD;
  spriteDef.color = makeARGB(255,255,255,255);

  spriteCmp.add(control[0], spriteDef);
  spriteCmp.add(control[1], spriteDef);
  spriteCmp.add(control[2], spriteDef);

  shapeDef.pos = RenderVec2( 500, 100);
  shapeDef.rect.upLeft = Vec2(0.f,0.f);
  shapeDef.rect.upRight = Vec2(50.f,0.f);
  shapeDef.rect.downLeft = Vec2(0.f,50.f);
  shapeDef.rect.downRight = Vec2(50.f,50.f);
  shapeCmp.overwrite(control[0], shapeDef);

  shapeDef.pos = RenderVec2( 700, 100);
  shapeCmp.overwrite(control[1], shapeDef);

  shapeDef.pos = RenderVec2( 200, 500);
  shapeCmp.overwrite(control[2], shapeDef);

  inputDef.callback = button1Callback;
  inputCmp.add(control[0], inputDef );
  inputCmp.add(control[1], inputDef );

  inputDef.callback = 0;
  //inputCmp.add(control[2], inputDef );


  GuiInfo guiInfo;
  guiInfo.action = incAndShow;
  guiCmp.add(control[0], guiInfo );


  guiInfo.action = decAndShow;
  guiCmp.add(control[1], guiInfo );

  guiInfo.action = 0;
  guiCmp.add(control[2], guiInfo );

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
		  inputCmp.update();
		  inputCmp.notifyAll(&event,keyState);

		  //guiCmp.checkAll(inputCmp.getMousePos() );
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
          }else if(event.button.button == SDL_BUTTON_RIGHT){
            //textCmp.setPosition(text[0], RenderVec2(event.button.x,event.button.y) );
            textCmp.get(text[0])->position =
              RenderVec2(event.button.x,event.button.y);
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

      inputCmp.update();
			inputCmp.notifyAll(0,keyState);

			movableCmp.update(TIME_STEP_S);

		}//koniec while (accumulator >= TIME_STEP_MS)
		movableCmp.clearForces();

    //inputCmp.update();

    ShapeDef* scd = shapeCmp.get(player);
    render->beginDraw( scd->pos );
    //render->beginDraw( RenderVec2() );

    static int s_a = 0;
    if(button1(control[2],this,&s_a) ){
      ++s_a;
    }

    spriteCmp.draw();
    textCmp.draw();

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

