#ifndef INPUTCMP_H
#define INPUTCMP_H

#include "component.h"

struct InputDef;
struct Game;

typedef void ( *InputCallback )( Game *, InputDef *, SDL_Event *, uint8_t * keyState );
typedef void ( *FrameCallback )( Game *, uint32_t frame, uint32_t frameProgress );

void inputCmpEventHandler( Game *, InputDef *, SDL_Event *, uint8_t * keyState );

struct InputDef {
  InputDef() : eventCallback(), frameCallback()
  {}
  InputDef( const InputDef & r, Entity e, Game * game );
  InputDef( char * line[] );

  std::string getAsString();
  void afterLoad(Game * game);

  Entity entity;
  //callback wywoływany gdy tylko przyjdzie jakiś event i w pętli fizyki
  InputCallback eventCallback;
  //callback wywoływanyraz na klatke frame
  FrameCallback frameCallback;

  std::string eventCallbackName,frameCallbackName;
};

/*
InputDef * inputDef = getRelated<InputDef>(entity);
vs.
InputDef * inputDef;        <----raczej to
getRelated(inputDef,entity);
vs.
InputDef * inputDef;
entity.getRelated(inputDef);
vs.
InputDef * inputDef = entity.getRelated<InputDef>();
*/

class InputCmp : public BaseComponent<InputDef, AddEmptyPolicy<InputDef> >
{
public:
  //mowi w ktorym momecie klatki(pętli głównej) jesteśmy
  enum FrameProgress {
    BEGIN,
    END,

    CALLS_PER_FRAME

  };
  typedef BaseComponent<InputDef, AddEmptyPolicy<InputDef> > BaseType;

  friend void inputCmpEventHandler( Game *, InputDef *, SDL_Event *, uint8_t * );

public:
  InputCmp( Game * game_ );
  virtual ~InputCmp();

  using BaseType::add;
  using BaseType::get;
  using BaseType::getOrAdd;
  using BaseType::overwrite;

  int32_t loadText( const char * filename, std::vector<Entity>& unit ) {
    int32_t r = BaseType::loadText( filename, unit );
    InputDef def;
    def.eventCallback = inputCmpEventHandler;
    overwrite( myEntity, def );
    return r;
  }

  void onFrameEnd();
  //wywoływana zarówno gdy jest event, oraz gdy takowego niema
  // do paru razy na klatke
  void notifyEventAll( SDL_Event * e, uint8_t * keyState );
  void notifyFrameProgressAll( uint32_t frame, uint32_t fs );

  const XY<int32_t>& getMousePos() const { return mousePos; };
  const XY<int32_t>& getPrevFrameMousePos() const
  { return prevFrameMousePos; };
  const XY<int32_t> getMousePosDelta() const
  {
    int x, y;
    SDL_GetMouseState( &x, &y );
    return mousePos - prevFrameMousePos;
    //return XY<int32_t>( x, y ) - prevFrameMousePos;
  };
  uint8_t getPrevFrameMouseState() const {
    return prevMouseState;
  }

  bool isButtonDown( uint32_t mouseButton ) ;
  bool isButtonReleased( uint32_t mouseButton ) ;
  int8_t getWheelDelta() {
    return wheel;
  }

protected:
  Entity myEntity;
  XY<int32_t> mousePos, prevFrameMousePos;
  uint8_t prevMouseState, mouseState;
  int8_t wheel;
};

template<>
inline
void getRelated<InputDef>(const Entity e,InputDef** out ){
  *out = g_game->getInputCmp()->get(e);
}

inline
bool isMouseButtonChanged( uint32_t button, uint8_t prevSt, uint8_t curSt )
{
  return ( prevSt ^ curSt ) & button;
}

inline
bool isMouseButtonPressed( uint32_t button, uint8_t prevSt, uint8_t curSt )
{
  return !( SDL_BUTTON( button ) & prevSt ) && ( SDL_BUTTON( button )&curSt );
}

inline
bool isMouseButtonReleased( uint32_t button, uint8_t prevSt, uint8_t curSt )
{
  return ( SDL_BUTTON( button ) & prevSt ) && !( SDL_BUTTON( button )&curSt );
}

#endif // INPUTCMP_H
