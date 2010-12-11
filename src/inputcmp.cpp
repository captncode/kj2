#include "inputcmp.h"

InputCmp::~InputCmp()
{
  //dtor
}

void InputCmp::update()
{
  prevFrameMousePos = mousePos;

  prevMouseState = mouseState;
  mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
}

void InputCmp::notifyAll(SDL_Event * e,uint8_t * keyState)
{
  for(int i = 0; i < (int)records.size(); ++i ){
  	records[i]->callback(game,records[i],e,keyState);
  }//koniec for (i)
}
