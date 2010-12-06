#include "mapcmp.h"
#include "main.h"
#include "render.h"

MapInfo::MapInfo( const MapDef & md, Entity entity_, Game * game ) : entity( entity_ )
{
  char * buffer = 0;
  size_t bufferSize = 0;
	if( loadFileToBuffer( md.filename.c_str(), &buffer, &bufferSize ) < 0) {
    PRINT_ERROR("nie zaladowano mapy: ");
    puts(md.filename.c_str() );
    return;
	}
	char * ptr = buffer;

	char tmp[250];

  sscanf(ptr,"%d %d %d",&startX,&startY,&startZ);

  ptr = strchr(ptr,'\n') + 1; //do nastepnej lini
  sscanf(ptr,"%d %d",&width,&height);

	ptr = strchr(ptr,'\n') + 1;//do nastepnej lini
	memset(tmp,0,250);
	sscanf(ptr,"%250s",tmp);
	filename = tmp;

	ptr = strchr(ptr,'\n') +1;
	memset(tmp,0,250);
	sscanf(ptr,"%250s",tmp);
	taiFilename = tmp;

	ptr = strchr(ptr,'\n') +1;

	std::vector<int> value;
	value.resize(width*height);
	for(int i = 0; i < (int)width*height; ++i ){
    int scanned = 0;
    /*int read = */sscanf(ptr,"%d",&scanned );
    value[i] = scanned;

    ptr = strchr(ptr,',') + 1;

	}/*koniec for (i)*/

	delete [] buffer;

  Render* r = game->getRender();
  atlasInfo = r->getAtlas(filename.c_str(),taiFilename.c_str() );
  if(!atlasInfo){
    atlasInfo = r->loadAtlas( filename.c_str(),taiFilename.c_str() );
    if(!atlasInfo){
      PRINT_ERROR("nie udalo sie zalodowac atlasu: ");
      fputs(filename.c_str(),stdout );
      puts(taiFilename.c_str() );
    }
  }

  MapVertex * vert = new MapVertex[width*height*4];
  int i = 0;
  for(int h = 0; h < (int)height; ++h ){ //poziome paski
    for(int w = 0; w < (int)width; ++w ){
      /*w + h*width = i/4*/
      Vec2Quad v2q = atlasInfo->getTileUV(value[w + h*width] );
      static const float zoomPx = 1.f;

      vert[i +0].pos =
        RenderVec3(w ,  h,  0.f)*zoomPx + RenderVec3(startX,startY,startZ);
      vert[i +0].t0 =  v2q.upLeft;

      vert[i +1].pos =
        RenderVec3(w+1,h,  0.f)*zoomPx + RenderVec3(startX,startY,startZ);
      vert[i +1].t0 =  v2q.upRight;

      vert[i +2].pos =
        RenderVec3(w+1,h+1,0.f)*zoomPx + RenderVec3(startX,startY,startZ);
      vert[i +2].t0 =  v2q.downRight;

      vert[i +3].pos =
        RenderVec3(w,  h+1,0.f)*zoomPx + RenderVec3(startX,startY,startZ);
      vert[i +3].t0 =  v2q.downLeft;
      i+= 4;
    }/*koniec for (w)*/
  }/*koniec for (h)*/

  vbo.bind();
  vbo.reserve(width*height*4,GL_STATIC_DRAW,vert);
  vbo.unbind();

  delete []vert;
}

MapCmp::MapCmp( Game * game ) : BaseComponent<MapInfo>( game )
{
  uint16_t * ind = new uint16_t[MAX_WIDTH*MAX_HEIGHT*6];
  int i = 0;
  int k = 0;
  for(int h = 0; h < (int)MAX_HEIGHT; h+=1 ){
    for(int w = 0; w < (int)MAX_WIDTH; ++w ){
      ind[k +0] = i+ 0;
      ind[k +1] = i+ 1;
      ind[k +2] = i+ 2;

      ind[k +3] = i+ 0;
      ind[k +4] = i+ 2;
      ind[k +5] = i+ 3;
      i+= 4;
      k+= 6;
    }/*koniec for (w)*/
  }/*koniec for (h)*/
  indexVbo.bind();
  indexVbo.reserve(MAX_WIDTH*MAX_HEIGHT*6,GL_STATIC_DRAW,ind);
  indexVbo.unbind();

  delete []ind;
}
MapCmp::~MapCmp()
{
	//dtor
}

void MapCmp::draw( const RenderVec2 & rv2 )
{
  glEnable(GL_TEXTURE_2D);

  indexVbo.bind();
  indexVbo.prepareDraw();
  for(int i = 0; i < (int)records.size(); ++i ){
    MapInfo * mi = records[i];

    glBindTexture(GL_TEXTURE_2D,mi->atlasInfo->tex);

    mi->vbo.bind();
    mi->vbo.prepareDraw();
    glDrawRangeElements(GL_TRIANGLES,0,indexVbo.capacity(),mi->width*mi->height*6,
                        GL_UNSIGNED_SHORT,0);
    mi->vbo.afterDraw();
    mi->vbo.unbind();
  }/*koniec for (i)*/
  indexVbo.afterDraw();
  indexVbo.unbind();
}
