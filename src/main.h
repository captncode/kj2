/*
 * main.h
 *
 *  Created on: 2010-11-12
 *      Author: Administrator
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "precompiled.h"
#include "entity.h"

class Render;

class InputCmp;
class SpriteCmp;
class TextCmp;
class ShapeCmp;
class MovableCmp;
class MapCmp;
class GuiCmp;
class GuiStyleCmp;

extern Entity nullEntity, gameEntity, g_Player, cursorId;

class Game
{
public:
  Game( int argc, char * argv[] );
  ~Game();

  int run();
  void end() { running = false; }
  const Entity createNextEntity()
  {
    unit.push_back( Entity(unit.size()) );
    return unit.back();
  }
/*private:*/ const Entity createGivenEntity( Entity e){
    assert( e.getId() < 0 );  //tylko ujemne
    for(int i = 0; i < (int)inCodeUnit.size(); ++i ){
      const Entity& e2 = inCodeUnit[i];
      if(e == e2)
      {
        assert( "Nie mozna utworzyc jednostki o tym ID: juz zajęte" );
        return nullEntity;
      }
    }/*koniec for (i)*/
    inCodeUnit.push_back( e );
    return e;
  }

public:
  void saveTextUnits( const char * name );
  void loadTextUnits( const char * name );
  void saveUnitsNumber( const char * filename );
  //przypisuje jednostce dest nowy numer source
  void resetEntity( Entity * dest, const Entity source );

  Render * getRender() { return render; }

  InputCmp * getInputCmp() { return  inputCmp; }
  SpriteCmp * getSpriteCmp() { return  spriteCmp; }
  TextCmp * getTextCmp() { return  textCmp; }
  ShapeCmp * getShapeCmp() { return  shapeCmp; }
  MovableCmp * getMovableCmp() { return  movableCmp; }
  MapCmp * getMapCmp()      { return mapCmp; }
  GuiCmp * getGuiCmp()      { return guiCmp; }
  GuiStyleCmp * getGuiStyleCmp() { return guiStyleCmp;}

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

public:
  std::vector<Entity> unit;
  std::vector<Entity> inCodeUnit; //ich numer jest znany w kodzie (stały)
  const Entity & player;

  Render * render;

//private: // czesto uzywam to moge je upublicznić
  //komponenty - trzymam wskazniki zeby przespieszyc kompilacje
  InputCmp  *inputCmp;
  SpriteCmp *spriteCmp;
  TextCmp   *textCmp;
  ShapeCmp  *shapeCmp;
  MovableCmp  *movableCmp;
  MapCmp    *mapCmp;
  GuiCmp    *guiCmp;
  GuiStyleCmp *guiStyleCmp;


  struct GuiData * guiData;
};//koniec klasy Game


extern Game * g_game;
#endif /* MAIN_H_ */
