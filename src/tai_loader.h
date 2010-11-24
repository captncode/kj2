#pragma once
#include "../src/precompiled.h"


#define MAX_CSTRING_LEN 100
#define ATLAS_TYPE_LEN  4

//! \struct TaiLineData
/*! */
struct TaiLineData {
  char filename[MAX_CSTRING_LEN];
  //char atlas_filename[MAX_CSTRING_LEN];     //to jest niepotrzebne
  float atlas_idx;
  char atlas_type[ATLAS_TYPE_LEN];
  float woffset;
  float hoffset;
  float depth_offset;
  float width;
  float height;
}; // koniec struktury TaiLineData

//! \class TaiLoader
/*! */
class TaiLoader
{
public:
  static const char taiFormat[];
  static const uint32_t fieldsToReadCount = 8;
public:
  TaiLoader( const char * name );
  ~TaiLoader();

  bool read( TaiLineData * pTld );
  bool isOk()const {
    return pBuffer && bufferSize && pLine;
  }

private:
  char * pBuffer;
  char * pLine;
  size_t bufferSize;
  uint32_t linesCount;        //!< ile lini zawiera dane
}; // koniec TaiLoader

struct TaiFilenameComp {
  //! podczas wywoływnia operator() with musi pozostać w zasięgu!
  TaiFilenameComp( const char * with ): pText( with ) {}
  bool operator()( const TaiLineData & l ) {
    //return !strncmp(pText,l.filename,MAX_CSTRING_LEN);
    return strstr( l.filename, pText );
  }
  const char * pText;
};

