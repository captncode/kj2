#include "CBFG_BitmapFontClass.h"
#include "../render.h"

//using namespace std;

#ifndef STRNLEN_DEFINED
#define STRNLEN_DEFINED
inline
uint32_t strnlen( const char * str, uint32_t maxLen )
{
  return std::min( strlen( str ), maxLen );
}
#endif /*STRNLEN_DEFINED*/

//!iloma spacjaami zastąpić taba
#define SPACES_PER_TAB 4

CBitmapFont::CBitmapFont( Render * pRender_ ) : pRender( pRender_ ), textDepth( 0.f )
{
  CurX = CurY = 0;
  Rd = Gr = Bl = 1.0f;
  InvertYAxis = false;
}

CBitmapFont::~CBitmapFont()
{

}

bool CBitmapFont::Load( const char * fname )
{
  char * dat, *img;
  //fstream in;
  FILE * in;
  unsigned long fileSize;
  char bpp;
  int ImgX, ImgY;

  //in.open(fname, ios_base::binary | ios_base::in);
  in = fopen( fname, "rb" );

  if( !in )
    return false;

  // Get Filesize
  //in.seekg(0,ios_base::end);
  //fileSize=in.tellg();
  //in.seekg(0,ios_base::beg);
  fseek( in, 0, SEEK_END );
  fileSize = ftell( in );
  fseek( in , 0 , SEEK_SET );



  // allocate space for file data
  dat = new char[fileSize];

  // Read filedata
  if( !dat )
    return false;

  //in.read(dat,fileSize);
  fread( dat, fileSize, 1, in );

  if( ferror( in ) ) {
    delete [] dat;
    fclose( in );
    return false;
  }

  fclose( in );

  // Check ID is 'BFF2'
  if(( unsigned char )dat[0] != 0xBF || ( unsigned char )dat[1] != 0xF2 ) {
    delete [] dat;
    return false;
  }

  // Grab the rest of the header
  memcpy( &ImgX, &dat[2], sizeof( int ) );
  memcpy( &ImgY, &dat[6], sizeof( int ) );
  memcpy( &CellX, &dat[10], sizeof( int ) );
  memcpy( &CellY, &dat[14], sizeof( int ) );
  bpp = dat[18];
  Base = dat[19];

  // Check filesize
  if( fileSize != (( MAP_DATA_OFFSET ) + (( ImgX * ImgY )*( bpp / 8 ) ) ) )
    return false;

  // Calculate font params
  RowPitch = ImgX / CellX;
  ColFactor = ( float )CellX / ( float )ImgX;
  RowFactor = ( float )CellY / ( float )ImgY;
  YOffset = CellY;

  // Determine blending options based on BPP
  switch( bpp ) {
    case 8: // Greyscale
      RenderStyle = BFG_RS_ALPHA;
      break;

    case 24: // RGB
      RenderStyle = BFG_RS_RGB;
      break;

    case 32: // RGBA
      RenderStyle = BFG_RS_RGBA;
      break;

    default: // Unsupported BPP
      delete [] dat;
      return false;
      break;
  }

  // Allocate space for image
  img = new char[( ImgX*ImgY )*( bpp/8 )];

  if( !img ) {
    delete [] dat;
    return false;
  }

  // Grab char widths
  memcpy( Width, &dat[WIDTH_DATA_OFFSET], 256 );

  // Grab image data
  memcpy( img, &dat[MAP_DATA_OFFSET], ( ImgX * ImgY )*( bpp / 8 ) );

  // Create Texture
  glGenTextures( 1, &TexID );
  glBindTexture( GL_TEXTURE_2D, TexID );
  // Fonts should be rendered at native resolution so no need for texture filtering
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // Stop chararcters from bleeding over edges
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // Tex creation params are dependent on BPP
  switch( RenderStyle ) {
    case BFG_RS_ALPHA:
      glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, ImgX, ImgY, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img );
      break;

    case BFG_RS_RGB:
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, ImgX, ImgY, 0, GL_RGB, GL_UNSIGNED_BYTE, img );
      break;

    case BFG_RS_RGBA:
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ImgX, ImgY, 0, GL_RGBA, GL_UNSIGNED_BYTE, img );
      break;
  }

  // Clean up
  delete [] img;
  delete [] dat;

  return true;
}

// Set the position for text output, this will be updated as text is printed
void CBitmapFont::SetCursor( int x, int y )
{
  CurX = x;
  CurY = y;
}

// The texture ID is a private member, so this function performs the texture bind
void CBitmapFont::Bind() const
{
  glBindTexture( GL_TEXTURE_2D, TexID );
}

// Set the color and blending options based on the Renderstyle member
void CBitmapFont::SetBlend() const
{
  glColor4b( Rd, Gr,Bl, Al);

  switch( RenderStyle ) {
    case BFG_RS_ALPHA: // 8Bit
      glBlendFunc( GL_SRC_ALPHA, GL_SRC_ALPHA );
      glEnable( GL_BLEND );
      break;

    case BFG_RS_RGB:   // 24Bit
      glDisable( GL_BLEND );
      break;

    case BFG_RS_RGBA:  // 32Bit
      glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
      glEnable( GL_BLEND );
      break;
  }
}

// Shortcut, Enables Texturing and performs Bind and SetBlend
void CBitmapFont::Select() const
{
  glEnable( GL_TEXTURE_2D );
  Bind();
  SetBlend();
}

// Set the font color NOTE this only sets the polygon color, the texture colors are fixed
void CBitmapFont::SetColor(uint8_t  Alpha,uint8_t Red, uint8_t Green,uint8_t Blue )
{
  Al = Alpha;
  Rd = Red;
  Gr = Green;
  Bl = Blue;
}

//
void CBitmapFont::ReverseYAxis( bool State )
{
  if( State )
    YOffset = -CellY;
  else
    YOffset = CellY;

  InvertYAxis = State;
}

// Sets up an Ortho screen based on the supplied values
void CBitmapFont::SetScreen( int x, int y )
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  if( InvertYAxis )
    glOrtho( 0, x, y, 0, -1, 1 );
  else
    glOrtho( 0, x, 0, y, -1, 1 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

// Prints text at the cursor position, cursor is moved to end of text
void CBitmapFont::Print( const char * Text, uint32_t coordSpace,int16_t depth)
{
  int sLen, Loop;
  int Row, Col;
  float U, V, U1, V1;

  sLen = ( int )strnlen( Text, BFG_MAXSTRING );

  for( Loop = 0; Loop != sLen; ++Loop ) {
    switch( Text[Loop] ) {
      case '\n':
        CurY += YOffset;     //line feed
        //CurX = 0;            //carrige return
        continue;
        break;
      case '\t':
        // szerokosc spacji razy ilosc domyslnych spacji wchodzacych na taba
        CurX += Width[' '] * SPACES_PER_TAB ;
        continue;
        break;
      case '\r':               //carrige return
        CurX = 0;
        continue;
        break;
      default:
        break;
    }//koniec switcxha

    Row = ( Text[Loop] - Base ) / RowPitch;
    Col = ( Text[Loop] - Base ) - Row * RowPitch;

    U = Col * ColFactor;
    V = Row * RowFactor;
    U1 = U + ColFactor;
    V1 = V + RowFactor;

    const Vec2Quad verts( RenderVec2( CurX, CurY ),
                          RenderVec2( CurX + CellX, CurY),
                          RenderVec2( CurX,      CurY + YOffset),  //leftDown
                          RenderVec2( CurX + CellX, CurY + YOffset) ); //rightDown
    const Vec2Quad tex( RenderVec2( U,  V ),
                                          RenderVec2( U1, V ),
                                          RenderVec2( U,  V1 ),
                                          RenderVec2( U1, V1 )
                                        );
    pRender->drawSprite( TexID,tex, verts, Render::CoordSpace_e(coordSpace),
                         makeARGB( Al, Rd, Gr, Bl ), depth );
    CurX += Width[Text[Loop]];
  }

}

// Prints text at a specifed position, again cursor is updated
void CBitmapFont::Print( const char * Text, float x, float y,
                        uint32_t coordSpace,int16_t depth )
{
  CurX = x;
  CurY = y;

  Print( Text, coordSpace, depth );
}
// Prints text at a specifed position and color, again cursor is updated
void CBitmapFont::Print( const char * Text, float x, float y,uint8_t a,
                        uint8_t r,uint8_t g,uint8_t b,uint32_t coordSpace,
                        int16_t depth)
{
  CurX = x;
  CurY = y;
  Al = a;
  Rd = r;
  Gr = g;
  Bl = b;

  Print( Text ,coordSpace ,depth );
}

//// Lazy way to draw text.
//// Preserves all GL attributes and does everything for you.
//// Performance could be an issue.
//void CBitmapFont::ezPrint( const char * Text, float x, float y )
//{
//  GLint CurMatrixMode;
//  GLint ViewPort[4];
//
//  // Save current setup
//  glGetIntegerv( GL_MATRIX_MODE, &CurMatrixMode );
//  glGetIntegerv( GL_VIEWPORT, ViewPort );
//  glPushAttrib( GL_ALL_ATTRIB_BITS );
//
//  // Setup projection
//  glMatrixMode( GL_PROJECTION );
//  glPushMatrix();
//  glLoadIdentity();
//  if( InvertYAxis )
//    glOrtho( 0, ViewPort[2], ViewPort[3], 0, -1, 1 );
//  else
//    glOrtho( 0, ViewPort[2], 0, ViewPort[3], -1, 1 );
//  glDisable( GL_DEPTH_TEST );
//  glDepthMask( false );
//
//  // Setup Modelview
//  glMatrixMode( GL_MODELVIEW );
//  glPushMatrix();
//  glLoadIdentity();
//
//  // Setup Texture, color and blend options
//  glEnable( GL_TEXTURE_2D );
//  glBindTexture( GL_TEXTURE_2D, TexID );
//  SetBlend();
//
//  // Render text
//  Print( Text, x, y );
//
//  // Restore previous state
//  glPopAttrib();
//
//  glMatrixMode( GL_PROJECTION );
//  glPopMatrix();
//  glMatrixMode( GL_MODELVIEW );
//  glPopMatrix();
//
//  glMatrixMode( CurMatrixMode );
//}

// Returns the width in pixels of the specified text
int CBitmapFont::GetWidth( const char * Text ) const
{
  int Loop, sLen, Size;

  // How many chars in string?
  sLen = ( int )strnlen( Text, BFG_MAXSTRING );

  // Add up all width values
  for( Loop = 0, Size = 0; Loop != sLen; ++Loop ) {
    Size += Width[Text[Loop]];
  }

  return Size;
}
