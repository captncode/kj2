#include "common.h"
#include "widget.h"
#include "main.h"


WindowXStyle::WindowXStyle(char* line[])
{
  memset( this,0,sizeof(*this) );
  backSpriteId = readEntityFormText(line[0]);
  barSpriteId = readEntityFormText(line[1]);
  borderSpriteId = readEntityFormText(line[2]);
  sscanf(line[3],"%f",&barHeight);
  sscanf(line[4],"%f",&borderWidth);
}

std::string WindowXStyle::getAsString() const
{
  char tmp[250] = {};
  std::string out = getEntityAsText(backSpriteId);
  out += " backSprite\n  ";
  out += getEntityAsText(barSpriteId);
  out += " barSprite\n  ";
  out += getEntityAsText(borderSpriteId);
  out += " borderSprite\n  ";
  memset(tmp,0,sizeof(tmp) );
  sprintf(tmp,"%f barHeight \n  %f borderWidth \n",barHeight,borderWidth);
  out += tmp;

  return out;
}


SliderXStyle::SliderXStyle(char* line[])
{
  memset( this,0,sizeof(*this) ); //epicki kontruktor :P
  upArrowSpriteId = readEntityFormText(line[0]);
  backSpriteId = readEntityFormText(line[1]);
  barSpriteId = readEntityFormText(line[2]);

  sscanf(line[3],"%f",&width);
}

std::string SliderXStyle::getAsString() const
{
  char tmp[250] = {};
  std::string out = getEntityAsText(upArrowSpriteId);
  out += " upArrowSprite\n  ";
  out += getEntityAsText(backSpriteId);
  out += " backSprite\n  ";
  out += getEntityAsText(barSpriteId);
  out += " barSprite\n  ";

  memset(tmp,0,sizeof(tmp) );
  sprintf(tmp,"%f width \n",width);
  out += tmp;

  return out;
}


ButtonXstyle::ButtonXstyle(char* line[])
{
}

std::string ButtonXstyle::getAsString() const
{
  GET_AS_STRING_BODY_BEGIN();
  GET_AS_STRING_BODY_END();
}



GuiStyleInfo::GuiStyleInfo()
{
}

GuiStyleInfo::GuiStyleInfo(const GuiStyleInfo& gi, Entity e, Game*)
{
  memcpy(this,&gi,sizeof(gi) );
}

GuiStyleInfo::GuiStyleInfo(char* line[])
{
  entity = readEntityFormText(line[0]);

  //char tmp[250] = {};

  const char * name[3] = {
    "windowX", "sliderX", "buttonX",
  };

  char bt []= " begin";
  int i = 1;
  while( strcmp("end entity",line[i]) != 0 )
  {
    int r = strncmp( bt, line[i],sizeof(bt)-1 );
    if( r == 0 ){
      for(int j = 0; j < sizeof(name)/sizeof(name[0]) ; ++j ){
        //printf("comparing '%s' and '%s'\n",name[j],line[i]+sizeof(bt) );
        int r2 = strcmp(name[j],line[i]+sizeof(bt) );
        if(r2 == 0){
          switch(j)
          {
          case 0:
            windowX = WindowXStyle(line+i+1);
            break;
          case 1:
            sliderX = SliderXStyle(line+i+1);
            break;
          case 2:
            buttonX = ButtonXstyle(line+i+1);
            break;
          default:
            break;
          }//switch j
          //konczy pętle
          j = sizeof(name)/sizeof(name[0]);
        }//if r2 == 0
      }/*koniec for (j)*/

    }//if r == 0
  ++i;
  }//while

}

GuiStyleInfo::~GuiStyleInfo()
{
}

std::string GuiStyleInfo::getAsString() const
{
  char tmp[250] = {};
  sprintf(tmp,"%u GuiStyleInfo entity\n",entity.getId() );
  std::string out (tmp);

  out += " begin windowX\n  ";
  out += windowX.getAsString();
  out += " end windowX\n\n";

  out += " begin sliderX\n  ";
  out += sliderX.getAsString();
  out += " end sliderX\n\n";

  out += " begin buttonX\n";
  out += buttonX.getAsString();
  out += " end buttonX";

  return out;
}

void GuiStyleInfo::afterLoad(Game* )
{
}

GuiStyleCmp::GuiStyleCmp(Game* game) : BaseType(game)
{
  myEntity = game->createEntity();
  GuiStyleInfo gsi; //domyślne ustawienia
  BaseType::overwrite(myEntity,gsi);

}
GuiStyleCmp::~GuiStyleCmp()
{
}
