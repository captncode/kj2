#include "timer.h"

BasicTimer::BasicTimer():
  lastTime( 0 ), delta( 0 )
{
  //ctor
}

BasicTimer::~BasicTimer()
{
  //dtor
}

void BasicTimer::start()
{
  lastTime = SDL_GetTicks() ;
  delta = 0;
}

