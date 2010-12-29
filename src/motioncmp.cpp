#include "motioncmp.h"
#include "main.h"

ShapeDef::ShapeDef( char * line[] )
{
  int e = 0;
  sscanf( line[0], "%i ", &e );
  entity = Entity( e );

  sscanf( line[1], "%f", &pos.x );
  sscanf( line[2], "%f", &pos.y );

  sscanf( line[3], "%f", &rect.upLeft.x );
  sscanf( line[4], "%f", &rect.upLeft.y );

  sscanf( line[5], "%f", &rect.upRight.x );
  sscanf( line[6], "%f", &rect.upRight.y );

  sscanf( line[7], "%f", &rect.downLeft.x );
  sscanf( line[8], "%f", &rect.downLeft.y );

  sscanf( line[9], "%f", &rect.downRight.x );
  sscanf( line[10], "%f", &rect.downRight.y );

  sscanf( line[11], "%f", &angle );
  angle *= toRadiansFactor;

  sscanf( line[12], "%hi", &depth );
  short vis;
  sscanf( line[13], "%hi", &vis );
  visible = vis;
}

std::string ShapeDef::getAsString() const
{
  char tmp[250] = {};
  char * ptr = tmp;


  sprintf( ptr, "%i %s\n", entity.getId(), "ShapeDef entity" );
  std::string out( tmp );
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f pos.x\n", pos.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f pos.y\n", pos.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.upLeft.x\n", rect.upLeft.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.upLeft.y\n", rect.upLeft.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.upRight.x\n", rect.upRight.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.upRight.y\n", rect.upRight.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.downLeft.x\n", rect.downLeft.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.downLeft.y\n", rect.downLeft.y );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.downRight.x\n", rect.downRight.x );
  out += tmp;
  memset( tmp, 0, 250 );

  sprintf( ptr, " %f rect.downRight.y\n", rect.downRight.y );
  out += tmp;
  memset( tmp, 0, 250 );

  float angleDeg = angle * toDegreesFactor;
  sprintf( ptr, " %f angle\n %hi depth\n", angleDeg,depth );
  out += tmp;
  memset( tmp, 0, 250 );

  short visible = this->visible;
  sprintf( ptr, " %hi visible\n", visible );
  out += tmp;

  return out;
}
void ShapeDef::afterLoad(Game * game)
{

}


MovableDef::MovableDef( char * line[] )
{
  int e;
  sscanf( line[0], "%i", &e );
  entity = Entity( e );

  sscanf( line[1], "%f %f", &v.x, &v.y );
  sscanf( line[2], "%f %f", &f.x, &f.y );
  sscanf( line[3], "%f %f", &m, &frictionFactor );
  sscanf( line[4], "%f", &maxVel );

}

std::string MovableDef::getAsString()
{
  std::string out;
  char tmp [250];

  memset( tmp, 0, 250 );
  sprintf( tmp, "%i Movable\n %f %f v.x v.y\n", entity.getId(), v.x, v.y );
  out += tmp;

  memset( tmp, 0, 250 );
  sprintf( tmp, " %f %f f.x f.y\n", f.x, f.y );
  out += tmp;

  memset( tmp, 0, 250 );
  sprintf( tmp, " %f %f m frictionFactor\n", m, frictionFactor );
  out += tmp;

  memset( tmp, 0, 250 );
  sprintf( tmp, " %f maxVel", maxVel );
  out += tmp;

  return out;
}

void MovableDef::afterLoad(Game * game)
{

}

MovableCmp::MovableCmp( Game * game_, uint32_t integrateStepCount ) :
  BaseComponent<MovableDef>( game_ ), stepCount( integrateStepCount )
{

}

MovableCmp::~MovableCmp()
{
  //dtor
}

void MovableCmp::add( Entity e, const MovableDef & mcd )
{
  //nie mozna mieć predkosci bez pozycji
  ShapeCmp * shcmp = game->getShapeCmp();
  if( !shcmp->get( e ) ) {
    ShapeDef scd;
    shcmp->add( e, scd );
  }
  BaseType::add( e, mcd );
}

void MovableCmp::collectForces()
{
  //gdy ciało ma niezerową prędkość dochodzi tarcie
  for( int i = 0; i < ( int )records.size(); ++i ) {
    MovableDef * md = records[i];
    if( md->v != RenderVec2() ) {
      md->f -= normalize( md->v ) * md->frictionFactor * 9.81f * md->m;
    }
  }/*koniec for (i)*/
}

void MovableCmp::update( float delta_s )
{
  const float delta2 = delta_s / float( stepCount );
  ShapeCmp * shcmp = game->getShapeCmp();
  for( int i = 0; i < ( int )stepCount; ++i )
  {
    for( __typeof( records.begin() ) it = records.begin(); it != records.end(); ++it ) {
      MovableDef * mcd = *it;
      ShapeDef * scd = shcmp->get( mcd->entity );
      if( mcd->m ) {
        mcd->v += ( mcd->f / mcd->m ) * delta2;
        if( mcd->maxVel )
        {
          Vec2 & v = mcd->v;
          //jakieś małe tarcie
          mcd->v -= Vec2( sign( v.x ) * 0.1f, sign( v.y ) * 0.1f );
          float lenVel = length( mcd->v );
          if( lenVel > mcd->maxVel ) {
            mcd->v *= ( mcd->maxVel / lenVel );
          }

          if( fabs( lenVel ) < 10e-1 )
            mcd->v = Vec2( 0.f, 0.f );
        }
        scd->pos += mcd->v * delta2;
        //printf("before rounding %f %f\n",scd->pos.x,scd->pos.y);
        scd->pos.x = roundComma(scd->pos.x,1);
        scd->pos.y = roundComma(scd->pos.y,1);
        //printf("after rounding %f %f\n",scd->pos.x,scd->pos.y);

      }
    }//koniec for(records)
  }//koniec for (i)

}

void MovableCmp::clearForces()
{
  for( __typeof( records.begin() ) it = records.begin(); it != records.end(); ++it ) {
    MovableDef * mcd = *it;
    mcd->f = Vec2( 0.f, 0.f );
  }
}
