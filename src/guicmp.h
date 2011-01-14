#pragma once
#include "precompiled.h"
#include "component.h"
#include "drawGui.h"

class Game;
class GuiAction;
struct GuiInfo;
struct GuiStyleInfo;
struct WidgetAction;
struct TextInfo;
struct ShapeDef;
struct AtlasInfo;

typedef uint32_t ( *GuiControl_pf )( Entity e, Game *,GuiInfo*);
typedef uint32_t ( *GuiRespond_pf  )( Entity e, Game *,GuiInfo*);


void calcWindowXUserRect(ShapeDef * shapeDef,GuiStyleInfo * gsi,
                         ShapeDef* out,int gotScroll);
//
//kontrolki
//uint32_t windowX( const Entity e, Game* game, GuiInfo * guiInfo);
//uint32_t sliderX( const Entity e, Game* game, GuiInfo * guiInfo);

void editX( const Entity e , Game* game ,WidgetAction * inout,
           ShapeDef * shape);
void consoleX( const Entity e, Game* game, WidgetAction* inout,
              ShapeDef* shape);

//akcje:
void drawMapTiles( const Entity e, Game* game,GuiInfo*guiInfo);


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
  GuiControl_pf widgetProcedure;

  GuiStyleInfo * styleInfo;
  ShapeDef * shapeDef;

  union{
    Widget common;
    WindowX windowX;
    SliderX sliderX;
    EditX editX;
    SpriteX spriteX;
    RectX rectX;
  };

}; /* koniec struktury GuiInfo*/

struct GuiAction {
  friend class GuiCmp;
private:
  GuiAction(): buttonState(), keyEntered(),keyUnicode(), keyMod()
  , mark(0) , cursor(0x7fff)
  {};
public:
  uint32_t buttonState;  /*!< maska bitowa z wartości GuiCmp::ControlFlag*/
  int16_t keyEntered;     /*! jaki klawisz klawiatury został wciśnięty*/
  int16_t keyUnicode; /*! zdekodowany klawisz do unikodu*/
  int32_t keyMod;   /*! modyfikator np. ctrl lub alt; KMOD_*/

  int16_t mark;      //!< poczatek zaznaczenia
  int16_t cursor;    //!< pozycja kursora tekstu wzgledem poczatku

  int16_t mouseX;
  int16_t mouseY;

  int16_t prevMouseX;
  int16_t prevMouseY;

  operator bool() {
    return buttonState || keyEntered;
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

    WHEEL_UP        = 0x100,
    WHEEL_DOWN      = 0x200,
  };
  enum TestValue{
    COLD = 0,
    HOT,
    ACTIVATED,
    ACTIVE,
    RELEASED,

    LOCKED_HOT,   //jesli inna kontrolka zablokowala focus
    BY_ME_LOCKED, //jesli obecna kontrolka zablokowała
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

  void focusNext();
  void focusPrev();

  TestValue actionTest( Entity e, ShapeDef * shapeDef);
  GuiAction& getState() { return state; }//uwaga! niestała referencja
  GuiAction& getPrevState() { return prevState; }

  bool isFocused( const Entity e ) {
    return kbdItem == e;
  }
  void makeFocused( const Entity e ) {
    kbdItem = e;
  }
private: TestValue makeHot(Entity e,ShapeDef* hotShape);
public: bool isHot(const Entity e)const {
    assert(e.getId() );
    return hot == e;
  }
public:
  bool isActive(const Entity e) const {
    //assert(e.getId() );
    return active == e;
  }
  /*! jak chce np. 2 razy pod rzad sprawdzic czy aktywna jest kontrolka z
      takim samym id, to czysze aktywną.
      justReleased nie jest ustawiana
      Dla dwóch takich samych ID dopiero po wyczyszczeniu mozna od
      actionTestu oczekiwać poprawnych wyników
      W sunie to w action teście moge wywoływać śmiało, bo czysci tylko jak
      podane ID jest takie same jak aktywne
  */
  void clearIfActive(Entity e){
    //jesli e == active to  (e!=active) = 0 * active.getId() = 0
    active = Entity( (bool)(e!=active)*active.getId() );
  }
  void clearIfHot(Entity e){
    //jesli e == active to  (e!=active) = 0 * active.getId() = 0
    hot = Entity( (bool)(e!=hot)*hot.getId() );
  }

  bool isJustReleased(const Entity e) const {
    return justReleased == e;
  }

  void lockHot(const Entity e,uint32_t lockReason_){
    assert(e.getId() );
    assert(lockReason_);
    locked = e;
    assert(locked.getId() != 0);

    this->lockReason = lockReason_;
    //printf("locking form %u\n",e.getId() );
  }
  //odblokowac moze tylo ten sam co zablokował
  uint32_t unlockHot(Entity e)  //dla Entity(0) poprostu zwraca reason
  {
    //printf("unlocking form %i\n",e.getId() );
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
  void setTestRegion(const ShapeDef& shape);
  void getTestRegion(ShapeDef * out) const;
  void setMouseTranslation(const Vec2& mt){
    mouseTranslation = mt;
  }
  Vec2 getMouseTranslation() const { return mouseTranslation; };

  void drawGui();

  void setVisibilityAll( bool v );

  // jesli kbdItem = notFocused wtedy nastepna kontrolka nie przechwyci focusa
  // bo ma go postac gracza
  static const Entity notFocused;

  //testRegion to obszar wewnątrz którego sprawdzane bedzie kliknięcie myszą
  ShapeDef * testRegion;
  Vec2 mouseTranslation;
protected:
  Entity guiId;

  /*! goracy to ten nad ktorym jest mysz. Kontrolka jest goraca gdy kursor
      jest nad nią i zadna inna kontrolka wczesniej nie stała się goraca
      Aktywna kontrolka to ta na którą kliknięto ( przycisk wciśnięty).
      justReleased jest ustawiana raz gdy kontrolka juz przestanie być atywna.
  */
  Entity hot, active, justReleased;

  /*! kbdItem to kontrolka która ma obecnie focus
      lastFocused to wcześniej wywoływana kontrolka
  */
  Entity kbdItem, lastTested;
  GuiAction state,prevState;//prevState to stan z poprzedniej klatki

  /*! id kontrolki która zarezerwowała sobie bycie gorącą*/
  Entity locked;
  uint32_t lockReason;  //! i powód dlaczego zablokowała sie jako gorąca

  int16_t hotDepth;   //!< na jakiej glebokosci lezy goraca kontrolka
  ShapeDef * hotShape;

}; /* koniec GuiCmp*/

template<>
inline
void getRelated<GuiInfo>(const Entity e,GuiInfo** out ){
  *out = g_game->getGuiCmp()->get(e);
}


