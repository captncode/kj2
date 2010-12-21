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
  InputDef( const InputDef & r, Entity e, Game * game ) :
    entity( e ), eventCallback( r.eventCallback )
    ,frameCallback( r.frameCallback )
  {}
  Entity entity;
  //callback wywoływany gdy tylko przyjdzie jakiś event i w pętli fizyki
  InputCallback eventCallback;
  //callback wywoływanyraz na klatke frame
  FrameCallback frameCallback;
};

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
  uint8_t wheel;
};

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

inline
void pop_back( std::string & s )
{
  s.resize( s.size() - 1 );
}

#endif // INPUTCMP_H
