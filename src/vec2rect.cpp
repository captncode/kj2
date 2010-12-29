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
