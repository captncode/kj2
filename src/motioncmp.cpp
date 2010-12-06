#include "motioncmp.h"
#include "main.h"

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
        float lenVel = length( mcd->v );
        if( mcd->maxVel && lenVel > mcd->maxVel ) {
          mcd->v *= ( mcd->maxVel/lenVel );
        }
        scd->pos += mcd->v * delta2;
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
