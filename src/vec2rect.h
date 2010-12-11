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

  //! http://www.alienryderflex.com/polygon/
  template <class T>
  bool isInside( const T & p )   const {

    static const PointType * poly = &upLeft;
    static const int32_t polySides = 4;
    int i, j = polySides - 1 ;
    bool oddNodes = false;

    for( i = 0; i < polySides; i++ ) {
      if( poly[i].y < p.y && poly[j].y >= p.y
          ||  poly[j].y < p.y && poly[i].y >= p.y ) {
        if( poly[i].x + ( p.y - poly[i].y ) / ( poly[j].y - poly[i].y )*( poly[j].x - poly[i].x ) < p.x ) {
          oddNodes = !oddNodes;
        }
      }
      j = i;
    }//koniec for(i)

    return oddNodes;
  }

  PointType upLeft,upRight,downRight,downLeft;
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
