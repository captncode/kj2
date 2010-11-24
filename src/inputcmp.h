#ifndef INPUTCMP_H
#define INPUTCMP_H

#include "component.h"

struct InputCmpData;
struct Game;

typedef void (*InputCallback)(Game*,InputCmpData * ,SDL_Event * e ,uint8_t * keyState);

struct InputCmpData{
  InputCmpData() : callback()
  {}
  InputCmpData(const InputCmpData& r,Entity e,Game* game) :
    entity(e),callback(r.callback)
  {}
  Entity entity;
  InputCallback callback;
};

class InputCmp : public BaseComponent<InputCmpData>
{
  typedef BaseComponent<InputCmpData> BaseType;
  public:
    InputCmp(Game * game_) : BaseComponent<InputCmpData>(game_) {}
    virtual ~InputCmp();

    using BaseType::add;

    void update();
    void handleEvent(SDL_Event * e,uint8_t * keyState);
  protected:
};

#endif // INPUTCMP_H
