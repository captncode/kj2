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
#include "mapcmp.h"
#include "guicmp.h"

extern Entity nullEntity;

void incAndShow(Game * game );
void decAndShow(Game * game );

class Game
{
  friend void incAndShow(Game *);//,decAndShow;
public:
	Game(int argc, char* argv[]);
	~Game();

	int run ();
	void end() { running = false; }

	Render * getRender() { return render; }
	InputCmp * getInputCmp() { return  &inputCmp; }
	SpriteCmp * getSpriteCmp() { return  &spriteCmp; }
	TextCmp * getTextCmp() { return  &textCmp; }
	ShapeCmp * getShapeCmp() { return  &shapeCmp; }
	MovableCmp * getMovableCmp() { return  &movableCmp; }
	MapCmp * getMapCmp()      { return &mapCmp; }
	GuiCmp * getGuiCmp()      { return &guiCmp; }

	static const uint32_t TIME_STEP_MS;
	static const float TIME_STEP_S;
private:
	SDL_Surface * mainSurf;
	bool running;

  Render * render;

	Entity gameEntity,player;
	Entity enemy[15];
	Entity map[20];
	Entity text[5];
	Entity control[50];

	//komponenty
	InputCmp  inputCmp;
	SpriteCmp spriteCmp;
	TextCmp   textCmp;
	ShapeCmp  shapeCmp;
	MovableCmp  movableCmp;
	MapCmp    mapCmp;
	GuiCmp    guiCmp;

};//koniec klasy Game


#endif /* MAIN_H_ */
