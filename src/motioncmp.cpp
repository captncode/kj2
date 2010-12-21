#include "motioncmp.h"
#include "main.h"

ShapeDef::ShapeDef( char* line[] )
{
  //pozniej to wyjebać koniecznie
  sscanf(line[0],"%i ",&entity );

  sscanf(line[1],"%f",&pos.x);
  sscanf(line[2],"%f",&pos.y);

  sscanf(line[3],"%f",&rect.upLeft.x);
  sscanf(line[4],"%f",&rect.upLeft.y);

  sscanf(line[5],"%f",&rect.upRight.x);
  sscanf(line[6],"%f",&rect.upRight.y);

  sscanf(line[7],"%f",&rect.downLeft.x);
  sscanf(line[8],"%f",&rect.downLeft.y);

  sscanf(line[9],"%f",&rect.downRight.x);
  sscanf(line[10],"%f",&rect.downRight.y);

  sscanf(line[11],"%hi",&depth);
  short vis;
  sscanf(line[12],"%hi",&vis);
  visible = vis;
}

std::string ShapeDef::getAsString() const
{
  char tmp[1024] = {};
  char * ptr = tmp;
#define SPRINTF_VAL_TO(dest,x,format) sprintf(dest," " format " " #x "\n" ,x)
  sprintf(ptr,"%i %s",entity.getId(), "ShapeDef entity" );
  ptr += sizeof("ShapeDef entity ");
  ptr += strlen(ptr);
  ptr++[0] = '\n';

  SPRINTF_VAL_TO(ptr,pos.x," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,pos.y," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.upLeft.x," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.upLeft.y," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.upRight.x," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.upRight.y," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.downLeft.x," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.downLeft.y," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,rect.downRight.x," %f" );
  ptr += strlen(ptr);
  assert(ptr < tmp+1024);

  SPRINTF_VAL_TO(ptr,rect.downRight.y," %f" );
  ptr += strlen(ptr);

  SPRINTF_VAL_TO(ptr,depth," %hi" );
  ptr += strlen(ptr);

  short visible = this->visible;
  SPRINTF_VAL_TO(ptr,visible," %hi" );
  assert(ptr < tmp+1024);

#undef SPRINTF_VAL_TO
  return tmp;
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
