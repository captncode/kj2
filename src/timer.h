#pragma once
#include "precompiled.h" //do sdl'a

/**
  \brief BasicTimer - stoper

  \author boo
  \date   19.VI.2010
*/

class BasicTimer
{
public:
  BasicTimer();
  ~BasicTimer();

  /*!włącza mierzenie czasu*/
  void start();

  void reset() {
    lastTime = SDL_GetTicks();
  }

  /*! jak dawno wywołano BasicTimer::start w ms*/
  uint32_t runningTime() const  { return SDL_GetTicks() - lastTime; };

protected:
  uint32_t lastTime, delta;
};

