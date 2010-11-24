#ifndef VEC2RECT_H
#define VEC2RECT_H

#include "common.h"

typedef Vec2 PointType;

class Vec2Quad
{
  public:
    Vec2Quad();
    Vec2Quad(PointType upLeft_,PointType upRight_,PointType downLeft_,
             PointType downRight_) :upLeft(upLeft_),upRight(upRight_),
             downLeft(downLeft_),downRight(downRight_)
    {}

  const PointType& at(int index)const  { return *((&upLeft)+index) ; }
  PointType& at(int index)  { return *((&upLeft)+index) ; }

  PointType upLeft,upRight,downLeft,downRight;
};

inline
void translateQuad(const Vec2Quad& q,Vec2 v,Vec2Quad * out){
  for(int i = 0; i < (int) 4; ++i ){
    out->at(i) = q.at(i) + v;
  }//koniec for (i)
}

//! \class Vec2AABB
/*! */
class Vec2AABB
{
public:
	Vec2AABB()
	{}
	Vec2AABB(PointType upLeft_,PointType downRight_) : upLeft(upLeft_),
            downRight(downRight_)
  {}

	~Vec2AABB(){}

	PointType upLeft,downRight;
}; // koniec Vec2AABB



#endif // VEC2RECT_H
