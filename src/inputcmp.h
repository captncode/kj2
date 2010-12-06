#ifndef INPUTCMP_H
#define INPUTCMP_H

#include "component.h"

struct InputDef;
struct Game;

typedef void (*InputCallback)(Game*,InputDef * ,SDL_Event * e ,uint8_t * keyState);

struct InputDef{
  InputDef() : callback()
  {}
  InputDef(const InputDef& r,Entity e,Game* game) :
    entity(e),callback(r.callback)
  {}
  Entity entity;
  InputCallback callback;
};

class InputCmp : public BaseComponent<InputDef>
{
  typedef BaseComponent<InputDef> BaseType;
  public:
    InputCmp(Game * game_) : BaseComponent<InputDef>(game_) {}
    virtual ~InputCmp();

    using BaseType::add;

    void update();
    void handleEvent(SDL_Event * e,uint8_t * keyState);
  protected:
};

#endif // INPUTCMP_H
