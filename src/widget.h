#pragma once
#include "guicmp.h"


/* Konstruktory bezparametorwe określają domyślny styl
*/

struct WindowXStyle{
  WindowXStyle() : barSpriteId(),borderSpriteId(),barHeight(10.f)
    ,borderWidth(1.f)
  {}
  WindowXStyle(char * line[]);
  std::string getAsString() const;

  Entity backSpriteId;
  Entity barSpriteId;
  Entity borderSpriteId;
  float barHeight;
  float borderWidth;
};

struct SliderXStyle{
  SliderXStyle() : upArrowSpriteId(),backSpriteId(),barSpriteId()
    ,width(10.f)
  {}
  SliderXStyle(char * line[]);
  std::string getAsString() const;

  Entity upArrowSpriteId;
  Entity backSpriteId;
  Entity barSpriteId;
  float width;
};

struct ButtonXstyle{
  ButtonXstyle(){
    memset(this,0,sizeof(*this) );
  }
  ButtonXstyle(char * line[]);
  std::string getAsString() const;

  //...
};

struct GuiStyleInfo
{
  GuiStyleInfo();
  GuiStyleInfo( const GuiStyleInfo & gi, Entity e, Game * );
  GuiStyleInfo( char* [] );
  ~GuiStyleInfo();
  std::string getAsString()const ;
  void afterLoad(Game * game);

  Entity entity;

  WindowXStyle windowX;
  SliderXStyle sliderX;
  ButtonXstyle buttonX;

};

//! \class GuiStyleCmp
/*! */
class GuiStyleCmp  : protected BaseComponent<GuiStyleInfo,
                                            ReturnZeroPolicy<GuiStyleInfo> >
{
  friend class Game;
public:
  typedef BaseComponent<GuiStyleInfo,ReturnZeroPolicy<GuiStyleInfo> > BaseType;
  GuiStyleCmp(Game* game);
  ~GuiStyleCmp();

  using BaseType::add;
  using BaseType::get;
  using BaseType::getSure;
  using BaseType::getOrAdd;

  int32_t loadText( const char * filename, std::vector<Entity>& unit )
  {
    int r = BaseType::loadText(filename,unit);
    GuiStyleInfo gsi; //domyślne ustawienia
    BaseType::overwrite(myEntity,gsi);
    return r;
  }

protected:
  Entity myEntity;
private:
}; /* koniec GuiStyleCmp*/
