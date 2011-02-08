#ifndef VEC2RECT_H
#define VEC2RECT_H

#include "common.h"

typedef Vec2 PointType;

/*! 1 - left up
    2 - right up
    3 - left down
    4 - right down
*/
int8_t whichPartRect(const Vec2& point, const Vec2& upLeft,
                     const Vec2& upRight,
                     const Vec2& downLeft,
                     const Vec2& downRight
                     );

class Vec2Quad
{
public:
  Vec2Quad();
  Vec2Quad( PointType upLeft_, PointType upRight_, PointType downLeft_,
            PointType downRight_ ) : upLeft( upLeft_ ), upRight( upRight_ ),
    downLeft( downLeft_ ), downRight( downRight_ )
  {}

  const PointType & at( int index )const  { return *(( &upLeft ) + index ) ; }
  PointType & at( int index )  { return *(( &upLeft ) + index ) ; }

  //! http://www.alienryderflex.com/polygon/
  template <class T>
  bool isInside( const T & p )   const {

    const PointType * poly = &upLeft;
    static const int32_t polySides = 4;
    int i, j = polySides - 1 ;
    bool oddNodes = false;

    for( i = 0; i < polySides; i++ ) {
      if( poly[i].y < p.y && poly[j].y >= p.y
          ||  poly[j].y < p.y && poly[i].y >= p.y ) {
        if( poly[i].x + ( p.y - poly[i].y ) / ( poly[j].y - poly[i].y ) * ( poly[j].x - poly[i].x ) < p.x ) {
          oddNodes = !oddNodes;
        }
      }
      j = i;
    }//koniec for(i)

    return oddNodes;
  }
  template <class T>
  bool isInside( float x,float y )   const {

    const PointType * poly = &upLeft;
    static const int32_t polySides = 4;
    int i, j = polySides - 1 ;
    bool oddNodes = false;

    for( i = 0; i < polySides; i++ ) {
      if( poly[i].y < y && poly[j].y >= y
          ||  poly[j].y < y && poly[i].y >= y ) {
        if( poly[i].x + ( y - poly[i].y ) / ( poly[j].y - poly[i].y ) * ( poly[j].x - poly[i].x ) < x ) {
          oddNodes = !oddNodes;
        }
      }
      j = i;
    }//koniec for(i)

    return oddNodes;
  }
  float width() const {return (upRight.x - upLeft.x); }
  float height() const { return downLeft.y - upLeft.y; }

  /*! 1 - left up
      2 - right up
      3 - left down
      4 - right down
  */
  int8_t whichPart(const Vec2& point ) const {
    return whichPartRect(point,upLeft,upRight,downLeft,downRight );
  }
  void rotateRight(){
    Vec2 tmp = upLeft;
    upLeft = downLeft;
    downLeft = downRight;
    downRight = upRight;
    upRight = tmp;
  }
  void rotateLeft(){
    Vec2 tmp = upLeft;
    upLeft = upRight;
    upRight = downRight;
    downRight = downLeft;
    downLeft = tmp;
  }

  std::string getAsString() const ;

  PointType upLeft, upRight, downRight, downLeft;
};

inline
void translateQuad( const Vec2Quad & q, Vec2 v, Vec2Quad * out ) {
  for( int i = 0; i < ( int ) 4; ++i ) {
    out->at( i ) = q.at( i ) + v;
  }//koniec for (i)
}

inline
void rotateQuad(const Vec2Quad& q,float angle,const Vec2& by,Vec2Quad* out)
{
  while (angle >= 2*PI){
    angle -= 2*PI;
  }
  for(int i = 0; i < (int)4; ++i ){
    rotateVec2(q.at(i),angle,by,&out->at(i) );
  }/*koniec for (i)*/
}

inline
void createRect(float width,float height,Vec2Quad * out)
{
  out->upLeft = Vec2(0.f,0.f);
  out->upRight = Vec2(width,0.f);
  out->downLeft = Vec2(0.f, height);
  out->downRight = Vec2( width,height );
}

//! \class Vec2AABB
/*! */
class Vec2AABB
{
public:
  Vec2AABB()
  {}
  Vec2AABB( PointType upLeft_, PointType downRight_ ) : upLeft( upLeft_ ),
    downRight( downRight_ )
  {}

  ~Vec2AABB() {}

  PointType upLeft, downRight;
}; // koniec Vec2AABB

#endif // VEC2RECT_H
