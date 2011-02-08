#include "vec2rect.h"
//#include <string>

Vec2Quad::Vec2Quad() : upLeft(), upRight(), downLeft(), downRight()
{
}

std::string Vec2Quad::getAsString() const
{
  char tmp[250] = {};

  const Vec2Quad & rect = *this;
  sprintf( tmp, " %f rect.upLeft.x\n", rect.upLeft.x );
  std::string out = tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.upLeft.y\n", rect.upLeft.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.upRight.x\n", rect.upRight.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.upRight.y\n", rect.upRight.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.downLeft.x\n", rect.downLeft.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.downLeft.y\n", rect.downLeft.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.downRight.x\n", rect.downRight.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( tmp, " %f rect.downRight.y\n", rect.downRight.y );
  out += tmp;

  return out;
}


int8_t whichPartRect(const Vec2& point, const Vec2& upLeft,
                     const Vec2& upRight,
                     const Vec2& downLeft,
                     const Vec2& downRight
                     )
{
  const float width = upRight.x - upLeft.x;
  const float height = downLeft.y - upLeft.y;

  uint8_t leftRight = 0;  //left = 1, right = 2, 0 = dunno;
  uint8_t upDown = 0;  //up = 1, down = 2, 0 = dunno;
  if( point.x > upLeft.x )
  {
    if( point.x < upLeft.x + 0.5f * width  )
    {
      leftRight = 1;
    }else if(point.x < upRight.x)
    {
      leftRight = 2;
    }else{
      return 8;
    }
  }
  if( point.y > upLeft.y ){
    if(point.y < upLeft.y + 0.5f * height ){
      upDown = 1;
    }else if(point.y < downLeft.y ){
      upDown = 2;
    }else{
      return 16;
    }
  }
  int8_t out = 1; //right up
  if( leftRight == 1){
    out = 2;
  }
  if(upDown == 1){
    out += 2;
  }

  return out;
}
