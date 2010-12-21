#include "inputcmp.h"
#include "main.h"

void inputCmpEventHandler( Game * game, InputDef * inputDef, SDL_Event * event,
                           uint8_t * keyState )
{
  if( !event ) return;
  InputCmp * inputCmp = game->getInputCmp();
  switch( event->type )
  {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      switch(event->button.button){
        case SDL_BUTTON_WHEELDOWN:
          inputCmp->wheel -= 1;
          break;
        case SDL_BUTTON_WHEELUP:
          inputCmp->wheel += 1;
          break;
      }//switch(event.button.button){
      break;//SDL_MOUSEBUTTONDOWN
      break;//SDL_MOUSEBUTTONUP

  }//switch (event.type )
}

InputCmp::InputCmp( Game * game_ ) : BaseType( game_ )
{
  InputDef def;
  def.eventCallback = inputCmpEventHandler;
  add( myEntity, def );
}

InputCmp::~InputCmp()
{
  //dtor
}

void InputCmp::onFrameEnd()
{
  prevFrameMousePos = mousePos;

  prevMouseState = mouseState;
  mouseState = SDL_GetMouseState( &mousePos.x, &mousePos.y );

  //wheel += -sign(wheel);
  wheel = 0;
}

void InputCmp::notifyEventAll( SDL_Event * e, uint8_t * keyState )
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    if(records[i]->eventCallback)
      records[i]->eventCallback( game, records[i], e, keyState );
  }//koniec for (i)
}
void InputCmp::notifyFrameProgressAll(uint32_t frame,uint32_t fs)
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    if(records[i]->frameCallback)
      records[i]->frameCallback( game, frame, fs );
  }//koniec for (i)
}

bool InputCmp::isButtonReleased(uint32_t mouseButton)
{
  //wczesniej wciśnięty, teraz zwolniony
  return (prevMouseState & SDL_BUTTON(mouseButton) &&
          !isButtonDown(mouseButton) );
}

bool InputCmp::isButtonDown(uint32_t mouseButton)
{
  prevMouseState = mouseState;
  mouseState = SDL_GetMouseState( &mousePos.x, &mousePos.y );
  return mouseState & SDL_BUTTON(mouseButton);
}
