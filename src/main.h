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

extern Entity nullEntity, gameEntity, g_Player;


class Game
{
public:
  Game( int argc, char * argv[] );
  ~Game();

  int run();
  void end() { running = false; }
  const Entity createEntity()
  {
    unit.push_back( Entity(unit.size()) );
    return unit.back();
  }

  void saveTextUnits( const char * name );
  void loadTextUnits( const char * name );
  void saveUnitsNumber( const char * filename );
  //przypisuje jednostce dest nowy numer source
  void resetEntity( Entity * dest, const Entity source );

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

  enum Modes {
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
  const Entity & player;


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
