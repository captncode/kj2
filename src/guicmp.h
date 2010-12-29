#pragma once
#include "precompiled.h"
#include "component.h"

class Game;


//! \struct GuiState
/*! */
struct GuiState
{
  GuiState();
  ~GuiState();

  //goracy to ten nad ktorym jest mysz, zerowany co klatke; kontrolka jest
  //goraca gdy kursor jest nad nią i zadna inna kontrolka wczesniej nie stała
  //się goraca
  Entity hot, active;
  //na jakiej glebokosci lezy goraca kontrolka
  int16_t hotDepth;
  //kbdItem to kontrolka która ma obecnie focus
  //lastFocused to kontrolka która miała w focus przed kbdItem
  Entity kbdItem, lastFocused;
  uint32_t mouse;
  int32_t keyEntered,keyMod;
  int16_t unicode;    //zdekodowany znak

}; /* koniec struktury GuiState*/

typedef void (*GuiRespond_pf)(Game * );

//! \struct GuiInfo
/*! */
struct GuiInfo
{
  GuiInfo();
  GuiInfo(const GuiInfo& gi,Entity e, Game * );
  GuiInfo( char* [] );
  ~GuiInfo();
  std::string getAsString()const ;

  Entity entity;
  bool isMoving;
  int16_t depth;

}; /* koniec struktury GuiInfo*/

struct GuiAction{
  GuiAction(): info(),key(),keyMod() {};
  uint32_t info;
  uint16_t key;
  uint16_t unicode; //zdekodowany klawisz

  uint8_t keyMod;
  operator bool(){
    return info || key;
  }
  struct TextInfo * textInfo;
  struct ShapeDef * shapeDef;
  GuiInfo * guiInfo;
} PACKED ;
void guiCmpInputHandler (Game*,struct InputDef * ,SDL_Event * e ,uint8_t * keyState);

//! \class GuiCmp
/*! */
class GuiCmp : public BaseComponent<GuiInfo,AddEmptyPolicy<GuiInfo> >
{
  friend void guiCmpInputHandler (Game*,InputDef * ,SDL_Event * e ,
                                  uint8_t * keyState);

public:
  /*! \brief Typ wyliczeniowy opisujacy stan i polożenie myszy.
  Kontrolka wywołując metode actonTest
  otrzymuje obiekt tego typu. Na podstawie ustawionych wartosci `pol bitowych`
  moze odpowiednio zareagowac*/
  enum ControlFlag{
    NONE = 0,

    LEFT_DOWN		  =	0x1,				/*!< wcisnieto lewy przycisk myszy*/
    MIDDLE_DOWN	  =	0x2,				/*!< wcisnieto kolko myszy*/
    RIGHT_DOWN		=	0x4,				/*!< wcisnieto prawy przycisk myszy*/
    ANY_DOWN	    =	LEFT_DOWN | MIDDLE_DOWN | RIGHT_DOWN,
    UNUSED_0			=	0x8,				/*!< zarezerwowane*/

    LEFT_RELEASED   =	0x10,				/*!< właśnie puszczono lewy przycisk myszy*/
    MIDDLE_RELEASED =	0x20,				/*!< puszczono srodkowy przycisk myszy*/
    RIGHT_RELEASED	=	0x40,				/*!< puszczono prawy przycisk myszy*/
    /*! suma flag puszconych klawiszy myszy*/
    ANY_RELEASED	  =	LEFT_RELEASED | MIDDLE_RELEASED | RIGHT_RELEASED,
    UNUSED2			  =	0x80,

    HOVER		    =	0x100,				/*!< mysz jest nad kontrolka*/
    WHEEL_UP		=	0x200,				/*!< krecił któś kółkiem? :P*/
    WHEEL_DOWN	=	0x400,				/*!< kolko w dol*/
    UNUSED4			=	0x800,				/*!< zarezerwowane*/

    FOCUSED			=	0x1000,				/*!< kontrolka ma focus*/
    UNUSED8			=	0x2000,
    UNUSED9			=	0x4000,
    HIDDEN			=	0x8000				/*!< kontrolka ukryta*/
  };

  typedef BaseComponent<GuiInfo,AddEmptyPolicy<GuiInfo> > BaseType;

  using BaseType::get;
  using BaseType::saveText;
  using BaseType::loadText;
  GuiCmp(Game * game);
  ~GuiCmp();

  GuiAction actionTest(Entity e);
  bool isFocused(const Entity e){
    return state.kbdItem == e;
  }

  void update();
  void setVisibilityAll(bool v);


  int16_t mark;      //poczatek zaznaczenia
  int16_t cursor;    //pozycja kursora

  // jesli kbdItem = notFocused wtedy nastepna kontrolka nie przechwyci focusa
  // bo ma go postac gracza
  static const Entity notFocused;
protected:
  Entity e;

  GuiState state,prevState;

}; /* koniec GuiCmp*/

bool makeControlMovable(GuiAction& a, const Vec2& mouseDelta,
                        GuiCmp::ControlFlag whenStart);
bool makeControlMovable(GuiAction& a, const XY<int32_t>& mouseDelta,
                        GuiCmp::ControlFlag whenStart);

void drawGui(Game * ,int32_t mode);
GuiAction editX( const Entity e , Game * game );
GuiAction controlPaneX( const Entity e , Game * game );
