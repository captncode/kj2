#pragma once
#include "main.h"

class Game;
class GuiInfo;
class AtlasInfo;

struct GuiData{
  int32_t rightMouseCell;
  int32_t leftMouseCell;
};

struct Widget{
  enum{
    NOTHING,
    JUST_DO_IT,
    HOVER,

    LEFT_DOWN,
    MIDDLE_DOWN,
    RIGHT_DOWN,
    WHEEL_UP,
    WHEEL_DOWN,

    LEFT_RELEASED,
    MIDDLE_RELEASED,
    RIGHT_RELEASED,
    WHEEL_UP_RELEASED,
    WHEEL_DOWN_RELEASED,


    COMMON_FIRST = 0xffff-1,
    MOVING,
    RESIZING,

    UP,
    DOWN,

  };
};
struct WindowX{

  float contentSizeX,contentSizeY;
  float contentBeginX,contentBeginY;
};

struct SliderX{
};
struct EditX{
};
struct SpriteX{

  const AtlasInfo * atlasInfo;
  uint32_t spriteId;

};

struct RectX
{
  int32_t spriteEntity;
};

typedef uint32_t ( *InsideWindowX_pf )( GuiInfo * windowGui,GuiData * );

uint32_t  sliderX( GuiInfo * guiInfo );
uint32_t  sliderX( const Entity e );

uint32_t  windowX_2( const Entity e );
uint32_t  windowX_2( GuiInfo * guiInfo,
                    GuiData* param = 0,InsideWindowX_pf toDo = 0 );

uint32_t rectX( const Entity e, Entity backId);
uint32_t rectX( GuiInfo* guiInfo, Entity backId);

uint32_t spriteX( const Entity e, GuiInfo * guiInfo);
uint32_t spriteX( GuiInfo * guiInfo);

void drawGui(Game* ,GuiData * );
