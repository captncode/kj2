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
    //wywoływana zarówno gdy jest event, oraz gdy takowego niema
    // do paru razy na klatke
    void notifyAll(SDL_Event * e,uint8_t * keyState);

    const XY<int32_t>& getMousePos() const { return mousePos; };
    const XY<int32_t>& getPrevFrameMousePos() const
      { return prevFrameMousePos; };
    const XY<int32_t> getMousePosDelta() const
      { return mousePos-prevFrameMousePos; };
    bool isButtonDown(uint32_t mouseButton) const {
      return mouseState & SDL_BUTTON(mouseButton);
    }
    bool isButtonReleased(uint32_t mouseButton) const {
      //wczesniej wciśnięty, teraz zwolniony
      return (prevMouseState & SDL_BUTTON(mouseButton) ) &&
        !isButtonDown(mouseButton);
    }

  protected:
    XY<int32_t> mousePos, prevFrameMousePos;
    uint8_t prevMouseState,mouseState;
};

#endif // INPUTCMP_H
