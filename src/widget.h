#pragma once
#include "guicmp.h"

bool makeControlMovable( GuiAction & a, const Vec2 & mouseDelta,
                         GuiCmp::ControlFlag whenStart );
bool makeControlMovable( GuiAction & a, const XY<int32_t>& mouseDelta,
                         GuiCmp::ControlFlag whenStart );

GuiAction consoleX( const Entity e, Game * game );
GuiAction windowX( const Entity e, Game * game );
void windowXCallback( Game * game, InputDef * inputDef, SDL_Event * event,
                           uint8_t * keyState );

GuiAction editX( const Entity e , Game * game );
GuiAction controlPaneX( const Entity e , Game * game );
