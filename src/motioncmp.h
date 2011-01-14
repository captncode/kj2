#ifndef MOVABLECMP_H
#define MOVABLECMP_H

#include "component.h"
#include "vec2rect.h"

struct Game;

struct ShapeDef
{
  ShapeDef() : pos(), rect(),angle(), depth(), visible( true )
  {}
  ShapeDef( const ShapeDef & r, Entity e, Game * ) :
    entity( e ), pos( r.pos ), rect( r.rect ),angle(r.angle), depth( r.depth ), visible( r.visible )
  {}
  ShapeDef( char* [] );
  std::string getAsString() const ;
  void afterLoad(Game * game);


  template <class T>
  bool isInside( const T & p )   const {
    return rect.isInside( p - T( pos ) );
  }
  Entity entity;
  Vec2 pos;
  Vec2Quad rect;
  //kąt pod jakim obrócony jest rect względem pos
  float angle;

  int16_t depth;
  bool visible;
};


//! \class ShapeCmp
/*! */
class ShapeCmp : public BaseComponent<ShapeDef, AddEmptyPolicy<ShapeDef> >
{
  typedef ShapeDef T;
  typedef BaseComponent<ShapeDef, AddEmptyPolicy<ShapeDef> > BaseType;
public:
  ShapeCmp( Game * game_ ) : BaseType( game_ ) {}
  ~ShapeCmp() {}

  using BaseType::add;
  using BaseType::get;
  using BaseType::getOrAdd;
  using BaseType::getSure;
  using BaseType::getNext;
  using BaseType::overwrite;
  using BaseType::saveText;
  using BaseType::loadText;
protected:
private:
}; // koniec ShapeCmp

template<>
inline
void getRelated<ShapeDef>(const Entity e,ShapeDef** out ){
  *out = g_game->getShapeCmp()->get(e);
}


struct MovableDef {
  MovableDef() : v(), f(), m(1.f), frictionFactor(), maxVel() {
  }
  MovableDef( const MovableDef & r, Entity e, Game * ) :
    entity( e ), v( r.v ), f( r.f ), m( r.m ), frictionFactor( r.frictionFactor )
    , maxVel( r.maxVel )
  {
  }
  MovableDef( char * line[] );
  std::string getAsString();
  void afterLoad(Game * game);

  void addAcc( const Vec2 & d ) {
    f += d * m;
  }
  void addForce( const Vec2 & d ) {
    f += d;
  }
  void addVel( const Vec2 & d ) {
    v += d;
  }
  void addMass( float d ) {
    m += d;
  }

  Entity entity;
  Vec2 v;
  Vec2 f;
  float m;

  float frictionFactor;
  float maxVel;
};
class MovableCmp : public BaseComponent<MovableDef>
{
  typedef MovableDef T;
  typedef BaseComponent<MovableDef> BaseType;
public:
  MovableCmp( Game * game_, uint32_t integrateStepCount );
  virtual ~MovableCmp();

  void add( Entity e, const MovableDef & mcd );

  using BaseType::get;

  void collectForces();
  void update( float delta_s );
  void clearForces();

protected:
  uint32_t stepCount;

};

template<>
inline
void getRelated<MovableDef>(const Entity e,MovableDef** out ){
  *out = g_game->getMovableCmp()->get(e);
}

#endif // MOVABLECMP_H
