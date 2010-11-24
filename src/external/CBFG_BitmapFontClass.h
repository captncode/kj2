#ifndef _BITMAPFONTCLASS_H
#define _BITMAPFONTCLASS_H

#include "../precompiled.h"


class Render;


#define BFG_RS_NONE  0x0      // Blend flags
#define BFG_RS_ALPHA 0x1
#define BFG_RS_RGB   0x2
#define BFG_RS_RGBA  0x4

#define BFG_MAXSTRING 1024     // Maximum string length - dojebałem

#define WIDTH_DATA_OFFSET  20 // Offset to width data with BFF file
#define MAP_DATA_OFFSET   276 // Offset to texture image data with BFF file

// This definition is missing from some GL libs
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

typedef struct {
  unsigned char ID1, ID2;
  unsigned char BPP;
  int ImageWidth, ImageHeight, CellWidth, CellHeight;
  unsigned char StartPoint;
} FontFileHeader;

class CBitmapFont
{
public:
  explicit CBitmapFont( Render * pRender );
  ~CBitmapFont();
  bool Load( const char * fname );
  void SetScreen( int x, int y );
  void SetCursor( int x, int y );
  //! zeby SetColor zadziałał trzeba wywołać SetBlend - czyli do dupy.
  void SetColor( float Red, float Green, float Blue );
  void ReverseYAxis( bool State );
  void Select() const ;
  void Bind() const ;
  void SetBlend() const ;
  void Print( const char * Text );
  void Print( const char * Text, float x, float y );
  void ezPrint( const char * Text, float x, float y );
  int  GetWidth( const char * Text ) const ;
  XYZ<float> GetColor() { return XYZ<float>( Rd, Gr, Bl ); }


private:
  int CellX, CellY, YOffset, RowPitch;
  char Base;
  char Width[256];
  GLuint TexID;
  float CurX, CurY;
  float RowFactor, ColFactor;
  int RenderStyle;
  float Rd, Gr, Bl;
  bool InvertYAxis;

  Render * pRender;
  float textDepth;
};

#endif
