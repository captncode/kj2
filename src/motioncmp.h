#ifndef MOVABLECMP_H
#define MOVABLECMP_H

#include "component.h"
#include "vec2rect.h"

struct Game;

struct ShapeCmpData
{
  ShapeCmpData() : pos(), shape()
  {}
  ShapeCmpData(const ShapeCmpData& r,Entity e,Game * ) :
    entity(e),pos(r.pos), shape(r.shape)
  {}
  Entity entity;
  Vec2 pos;
  Vec2Quad shape;

};

//! \class ShapeCmp
/*! */
class ShapeCmp : public BaseComponent<ShapeCmpData>
{
  typedef ShapeCmpData T;
  typedef BaseComponent<ShapeCmpData> BaseType;
public:
	ShapeCmp(Game * game_) : BaseComponent<ShapeCmpData>(game_){}
	~ShapeCmp(){}

  using BaseType::add;
	using BaseType::get;
  using BaseType::overwrite;
protected:
private:
}; // koniec ShapeCmp

struct MovableCmpData{
  MovableCmpData() : v(),f(),m(),r() {
  }
  MovableCmpData(const MovableCmpData& r,Entity e,Game * ) :
    entity(e),v(r.v),f(r.f),m(r.m),r(r.r) {
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
};
class MovableCmp : public BaseComponent<MovableCmpData>
{
  typedef MovableCmpData T;
  typedef BaseComponent<MovableCmpData> BaseType;
  public:
    MovableCmp(Game * game_,uint32_t integrateStepCount);
    virtual ~MovableCmp();

    void add(Entity e,const MovableCmpData& mcd);

    T* get(Entity e){
      return BaseType::get(e);
    }

    void collectForces(){}
    void update(float delta_s);
    void clearForces();

  protected:
  uint32_t stepCount;

};

#endif // MOVABLECMP_H
