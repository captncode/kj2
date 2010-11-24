/*
 * main.h
 *
 *  Created on: 2010-11-12
 *      Author: Administrator
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "precompiled.h"
#include "render.h"
#include "inputcmp.h"
#include "motioncmp.h"

class Game
{
public:
	Game(int argc, char* argv[]);
	~Game();

	int run ();
	void end() { running = false; }

	Render * getRender() { return render; }
	ShapeCmp * getShapeCmp() { return  &shapeCmp; }
	MovableCmp * getMovableCmp() { return  &movableCmp; }

	static const uint32_t TIME_STEP_MS;
	static const float TIME_STEP_S;
private:
	SDL_Surface * mainSurf;
	bool running;

  Render * render;

	Entity gameEntity,player;
	Entity enemy[150];
	//komponenty
	InputCmp inputCmp;
	SpriteCmp spriteCmp;
	ShapeCmp shapeCmp;
	MovableCmp movableCmp;

};//koniec klasy Game


#endif /* MAIN_H_ */
