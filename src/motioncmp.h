#ifndef MOVABLECMP_H
#define MOVABLECMP_H

#include "component.h"
#include "vec2rect.h"

struct Game;

struct ShapeDef
{
  ShapeDef() : pos(), rect(),depth()
  {}
  ShapeDef(const ShapeDef& r,Entity e,Game * ) :
    entity(e),pos(r.pos), rect(r.rect), depth(r.depth)
  {}
  Entity entity;
  Vec2 pos;
  Vec2Quad rect;

  int16_t depth;
};

//! \class ShapeCmp
/*! */
class ShapeCmp : public BaseComponent<ShapeDef>
{
  typedef ShapeDef T;
  typedef BaseComponent<ShapeDef> BaseType;
public:
	ShapeCmp(Game * game_) : BaseComponent<ShapeDef>(game_){}
	~ShapeCmp(){}

  using BaseType::add;
	using BaseType::get;
  using BaseType::overwrite;
protected:
private:
}; // koniec ShapeCmp

struct MovableDef{
  MovableDef() : v(),f(),m(),r(),frictionFactor(),maxVel() {
  }
  MovableDef(const MovableDef& r,Entity e,Game * ) :
    entity(e),v(r.v),f(r.f),m(r.m),r(r.r),frictionFactor(r.frictionFactor)
    ,maxVel(r.maxVel)
  {
  }
  void addAcc(const Vec2& d){
    f += d * m;
  }
  void addForce(const Vec2& d){
    f += d;
  }
  void addVel(const Vec2& d){
    v += d;
  }
  void addRay(float d){
    r += d;
  }
  void addMass(float d){
    m += d;
  }

  Entity entity;
  Vec2 v;
  Vec2 f;
  float m;
  float r;    //jesli cos jest w kształcie kola to ma niezerowy promien

  float frictionFactor;
  float maxVel;
};
class MovableCmp : public BaseComponent<MovableDef>
{
  typedef MovableDef T;
  typedef BaseComponent<MovableDef> BaseType;
  public:
    MovableCmp(Game * game_,uint32_t integrateStepCount);
    virtual ~MovableCmp();

    void add(Entity e,const MovableDef& mcd);

    T* get(Entity e){
      return BaseType::get(e);
    }

    void collectForces();
    void update(float delta_s);
    void clearForces();

  protected:
  uint32_t stepCount;

};

#endif // MOVABLECMP_H
