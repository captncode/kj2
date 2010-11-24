#include "motioncmp.h"
#include "main.h"

MovableCmp::MovableCmp(Game * game_,uint32_t integrateStepCount) :
      BaseComponent<MovableCmpData>(game_),stepCount(integrateStepCount)
{

}

MovableCmp::~MovableCmp()
{
  //dtor
}

void MovableCmp::add(Entity e,const MovableCmpData& mcd)
{
  //nie mozna mieć predkosci bez pozycji
  ShapeCmp * shcmp = game->getShapeCmp();
  if( !shcmp->get(e) ){
    ShapeCmpData scd;
    shcmp->add(e,scd);
  }
  BaseType::add(e,mcd);
}

void MovableCmp::update(float delta_s)
{
  const float delta2 = delta_s / float(stepCount);
  ShapeCmp * shcmp = game->getShapeCmp();
  for(int i = 0; i < (int)stepCount; ++i )
  {
    for( __typeof(records.begin()) it = records.begin(); it != records.end(); ++it ){
      MovableCmpData * mcd = *it;
      ShapeCmpData * scd = shcmp->get(mcd->entity);
      if(mcd->m){
        mcd->v += (mcd->f/mcd->m) *delta2;
        scd->pos += mcd->v *delta2;
      }
    }//koniec for(records)
  }//koniec for (i)

}
void MovableCmp::clearForces()
{
  for( __typeof(records.begin()) it = records.begin(); it != records.end(); ++it ){
  	MovableCmpData * mcd = *it;
  	mcd->f = Vec2(0.f,0.f);
  }
}
