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


class Game
{
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

	enum Modes{
    NORMAL,
    EDIT
	};
private:
	SDL_Surface * mainSurf;
	bool running;
	uint8_t mode;

  Render * render;

public:
  std::vector<Entity> unit;
  static const Entity gameEntity;
  const Entity player;


private:
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
