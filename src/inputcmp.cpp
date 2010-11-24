#include "inputcmp.h"

InputCmp::~InputCmp()
{
  //dtor
}

void InputCmp::update()
{
}

void InputCmp::handleEvent(SDL_Event * e,uint8_t * keyState)
{
  for(int i = 0; i < (int)records.size(); ++i ){
  	records[i]->callback(game,records[i],e,keyState);
  }//koniec for (i)
}
