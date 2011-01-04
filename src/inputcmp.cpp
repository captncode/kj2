#include "inputcmp.h"
#include "main.h"
#include "parser.h"

void inputCmpEventHandler( Game * game, InputDef * inputDef, SDL_Event * event,
                           uint8_t * keyState )
{
  if( !event ) return;
  InputCmp * inputCmp = game->getInputCmp();
  switch( event->type )
  {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      switch( event->button.button ) {
        case SDL_BUTTON_WHEELDOWN:
          inputCmp->wheel -= 1;
          break;
        case SDL_BUTTON_WHEELUP:
          inputCmp->wheel += 1;
          break;
      }//switch(event.button.button){
      break;//SDL_MOUSEBUTTONDOWN
      break;//SDL_MOUSEBUTTONUP

    case SDL_MOUSEMOTION:
      //jak sie delta muszki bedzie jebać pamiętać ab użyć tych pól:
      /*
      event->motion.xrel
      event->motion.yrel
      */
      break;

  }//switch (event.type )
}

InputDef::InputDef( const InputDef & r, Entity e, Game * game ) :
  entity( e )
  , eventCallback( r.eventCallback )
  , frameCallback( r.frameCallback )
  , eventCallbackName( r.eventCallbackName )
  , frameCallbackName( r.frameCallbackName )
{
  parserNs::BaseParserItem * bpi = parser.getFunction( eventCallbackName.c_str() );
  if( bpi ) {
    eventCallback = bpi->getPointer<InputCallback>();
    assert( eventCallback );
  }
  bpi = parser.getFunction( frameCallbackName.c_str() );
  if( bpi ) {
    frameCallback = bpi->getPointer<FrameCallback>();
    assert( frameCallback );
  }
}

InputDef::InputDef( char * line[] ) : eventCallback( 0 ), frameCallback( 0 )
{
  uint32_t e = 0;
  sscanf( line[0], "%u", &e );
  assert( e );
  entity = Entity( e );

  char tmp [250] = {};
  sscanf( line[1], " %s", tmp );
  eventCallbackName = tmp;

  memset( tmp, 0, 255 );
  sscanf( line[2], " %s", tmp );
  frameCallbackName = tmp;


}
std::string InputDef::getAsString()
{
  std::string out;
  char tmp[250];

  memset( tmp, 0, 250 );
  sprintf( tmp, "%i InputDef\n", entity.getId() );
  out += tmp;

  if( eventCallbackName.size() > 0 ) {
    memset( tmp, 0, 250 );
    sprintf( tmp, " %s eventCallback\n", eventCallbackName.c_str() );
    out += tmp;
  }
  else {
    out += " 0 eventCallback\n";
  }

  if( frameCallbackName.size() > 0 ) {
    memset( tmp, 0, 250 );
    sprintf( tmp, " %s frameCallback", frameCallbackName.c_str() );
    out += tmp;
  }
  else {
    out += " 0 frameCallback\n";
  }

  return out;
}
void InputDef::afterLoad( Game * game )
{

}


InputCmp::InputCmp( Game * game_ ) : BaseType( game_ )
{
  InputDef def;
  def.eventCallback = inputCmpEventHandler;
  myEntity = game->createEntity();
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
    if( records[i]->eventCallback )
      records[i]->eventCallback( game, records[i], e, keyState );
  }//koniec for (i)
}
void InputCmp::notifyFrameProgressAll( uint32_t frame, uint32_t fs )
{
  for( int i = 0; i < ( int )records.size(); ++i ) {
    if( records[i]->frameCallback )
      records[i]->frameCallback( game, frame, fs );
  }//koniec for (i)
}

bool InputCmp::isButtonReleased( uint32_t mouseButton )
{
  //wczesniej wciśnięty, teraz zwolniony
  return ( prevMouseState & SDL_BUTTON( mouseButton ) &&
           !isButtonDown( mouseButton ) );
}

bool InputCmp::isButtonDown( uint32_t mouseButton )
{
  prevMouseState = mouseState;
  mouseState = SDL_GetMouseState( &mousePos.x, &mousePos.y );
  return mouseState & SDL_BUTTON( mouseButton );
}
