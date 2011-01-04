#pragma once
#include "precompiled.h"
#include "component.h"

class Game;
class GuiAction;
struct GuiInfo;
struct GuiStyleInfo;
struct WidgetAction;
struct TextInfo;
struct ShapeDef;
struct AtlasInfo;

typedef void ( *GuiControl_pf )( Entity e, Game *,WidgetAction*,GuiInfo*);
typedef void ( *GuiRespond_pf  )( Entity e, Game *,WidgetAction*,GuiInfo*);


struct WidgetCommon{
  enum{
    NOTHING,
    MOVE,
    RESIZE_TOP,
    RESIZE_BOTTOM,
    RESIZE_LEFT,
    RESIZE_RIGHT,

    COMMON_LAST = 0xffff-1
  };
};
struct WindowX{
  enum {
    NONE = WidgetCommon::COMMON_LAST,
  };
  float contentSizeX,contentSizeY;
  float scrollHor,scrollVer;
};

struct SliderX{
  enum
  {
    NONE = WidgetCommon::COMMON_LAST,
    UP_RIGHT_CLICKED,
    DOWN_LEFT_CLICKED,
    WHEEL_UP,
    WHEEL_DOWN
  };
};
struct EditX{
  enum{
    NONE = WidgetCommon::COMMON_LAST,
  };
};
struct SpriteX{
  enum{
    NONE = WidgetCommon::COMMON_LAST,
  };
  const AtlasInfo * atlasInfo;
  uint32_t spriteId;

};

//kontrolki
void windowX( const Entity e, Game* game, WidgetAction* inout,
             GuiInfo * guiInfo);
void sliderX( const Entity e, Game* game ,WidgetAction* inout,
             ShapeDef * shape);
void spriteX( const Entity e, Game * game, WidgetAction * inout,
                   GuiInfo * guiInfo);

void editX( const Entity e , Game* game ,WidgetAction * inout,
           ShapeDef * shape);
void consoleX( const Entity e, Game* game, WidgetAction* inout,
              ShapeDef* shape);

//akcje:
void drawMapTiles( const Entity e, Game* game, WidgetAction* inout,
                  GuiInfo*guiInfo);


//! \struct GuiInfo
/*! */
struct GuiInfo
{
  enum State_e{
    NONE                = 0,
    MOVING              = 1,
    RESIZING_LEFT       = 1<<1,
    RESIZING_RIGHT      = 1<<2,
    RESIZING_TOP        = 1<<3,

    RESIZING_BOTTOM     = 1<<4,



    ALL_RESIZING        = (RESIZING_BOTTOM | RESIZING_TOP | RESIZING_LEFT |
                           RESIZING_RIGHT),

  };
  //using State_e;
  GuiInfo();
  GuiInfo( const GuiInfo & gi, Entity e, Game * );
  GuiInfo( char* [] );
  ~GuiInfo();
  std::string getAsString()const ;
  void afterLoad(Game * game);

  Entity entity;
  GuiControl_pf guiControl;
  GuiRespond_pf guiRespond;
  std::string controlType, respondName;
  uint32_t state;

  Entity shapeId;
  Entity styleId;

  GuiStyleInfo * styleInfo;
  ShapeDef * shapeDef;

  union WidgetData{
    WidgetCommon common;
    WindowX windowX;
    SliderX sliderX;
    EditX editX;
    SpriteX spriteX;
  };
  WidgetData widgetData;

}; /* koniec struktury GuiInfo*/



struct GuiAction {
  GuiAction(): hotInfo(), key(),unicode(), keyMod()
  {};
  uint32_t hotInfo;  /*!< maska bitowa z wartości GuiCmp::ControlFlag*/
  uint16_t key;     /*! jaki klawisz klawiatury został wciśnięty*/
  uint16_t unicode; /*! zdekodowany klawisz do unikodu*/

  uint8_t keyMod;   /*! modyfikator np. ctrl lub alt; KMOD_*/
  operator bool() {
    return hotInfo || key;
  }

} /*PACKED*/ ;

/*
Kazda kontrolka w reakcji na input modyfikuje swój stan, ewentualnie stan
zmiennej. Kontrolka nie dostaje informacji o zmiennej, bo jej typ jest
definiowany na zewnątrz (w pliku). Dąże do rozdzielenia rysowania kontrolki i
przechwytywania wejścia użytkownika od modyfikacji powiązanych danych.
Pomysł polega na zwracaniu przez kontrolke po narysowaniu odpowiednich
wartośći, które to w zależności od kontrolki będą oznaczały podjętą akcje.
Wymaga to dokładnego określenia zwracanej wartości od podjętej akcji, dla
każdej kontrolki
np. okno wysyła komunikat ze zostało kliknięte, jesli istotnie jest którym
klawiszem szczegóły znajdują się w polu details.

Podsumowując kontrolka przerabia dane o kliknięciu (input) na informacje co
należy wykonać, następnie inna procedura dostaje tą informacjie i przetwarza
na jej podstawie dane użytkownika.

EDIT: wszystko ładnie pięknie, tylko schody zaczynają sie gdy kontrolka zależy
od danych użytkownika (np. okno wyświetlające dane usera)
*/
struct WidgetAction : public GuiAction
{
  WidgetAction(const GuiAction& ga) : u32(), msg(0)
  {
    memcpy(this,&ga,sizeof(ga) );
  }
  //mozna ten kostruktor ukryć
  WidgetAction() :GuiAction(), u32(),msg(0)
  {
  }

  union{
    uint32_t  u32;
    int32_t   i32;
    int16_t   i16;
    uint16_t  u16;
    int8_t    i8;
    uint8_t   u8;
    float     f;    //jebać doubla
    bool      b;
    void*     v;
  }/*value*/;
  uint32_t msg;
};

void guiCmpInputHandler( Game *, struct InputDef * , SDL_Event * e ,
                         uint8_t * keyState );

//! \class GuiCmp
/*! */
class GuiCmp : public BaseComponent<GuiInfo, AddEmptyPolicy<GuiInfo> >
{
  friend void guiCmpInputHandler( Game *, InputDef * , SDL_Event * e ,
                                  uint8_t * keyState );

public:
  /*! \brief Typ wyliczeniowy opisujacy stan i polożenie myszy.
  Kontrolka wywołując metode actonTest
  otrzymuje obiekt tego typu. Na podstawie ustawionych wartosci `pol bitowych`
  moze odpowiednio zareagowac*/
  enum ControlFlag {
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

  typedef BaseComponent<GuiInfo, AddEmptyPolicy<GuiInfo> > BaseType;

  using BaseType::get;
  using BaseType::getOrAdd;
  using BaseType::getSure;
  using BaseType::saveText;
  using BaseType::overwrite;
  //using BaseType::loadText;
  int32_t loadText( const char * filename, std::vector<Entity>& unit );

  GuiCmp( Game * game );
  ~GuiCmp();

  void initFrame(ShapeDef * windowShape);

  void focusNext( GuiAction * ai );
  void focusPrev( GuiAction * ai );

  GuiAction actionTest( Entity e, ShapeDef * shapeDef);
  bool isFocused( const Entity e ) {
    return kbdItem == e;
  }
  void makeFocused( const Entity e ) {
    kbdItem = e;
  }
  bool isHot(const Entity e)const {
    assert(e.getId() );
    return hot == e;
  }
  bool isActive(const Entity e) const {
    assert(e.getId() );
    return active == e;
  }
  const GuiAction& getActiveDetails(){
    assert(active.getId() );
    return activeDetails;
  }

  void lockHot(const Entity e,uint32_t lockReason_){
    assert(e.getId() );
    assert(lockReason_);
    locked = e;
    this->lockReason = lockReason_;
  }
  //odblokowac moze tylo ten sam co zablokował
  uint32_t unlockHot(Entity e)  //dla Entity(0) poprostu zwraca reason
  {
    assert(e.getId() );
//    if(e == locked)
//      locked = Entity(0);
    //uwaga mega optymalizacja :D
    locked = Entity( (bool)(e != locked)*locked.getId() );
    return lockReason;
  }
  uint32_t getLockReason() const {
    return lockReason;
  }
  bool isLocked(Entity e) const {
    return locked == e;
  }

  void drawGui();

  void update();
  void setVisibilityAll( bool v );


  int16_t mark;      //poczatek zaznaczenia
  int16_t cursor;    //pozycja kursora
  uint32_t hotState; // stan gorącej kontrolki

  // jesli kbdItem = notFocused wtedy nastepna kontrolka nie przechwyci focusa
  // bo ma go postac gracza
  static const Entity notFocused;
protected:
  Entity e;

  /*! goracy to ten nad ktorym jest mysz, zerowany co klatke; kontrolka jest
      goraca gdy kursor jest nad nią i zadna inna kontrolka wczesniej nie stała
      się goraca
  */
  Entity hot, active;
  //! na jakiej glebokosci lezy goraca kontrolka
  int16_t hotDepth;
  /*! kbdItem to kontrolka która ma obecnie focus
      lastFocused to kontrolka która miała w focus przed kbdItem
  */
  Entity kbdItem, lastFocused;
  uint32_t mouse;
  int32_t keyEntered, keyMod;
  int16_t unicode;    //zdekodowany znak

  /*! id kontrolki która zarezerwowała sobie bycie gorącą*/
  Entity locked;
  //! i powód dlaczego zablokowała sie jako gorąca
  uint32_t lockReason;

  ShapeDef * hotShape;

  GuiAction activeDetails;

}; /* koniec GuiCmp*/


