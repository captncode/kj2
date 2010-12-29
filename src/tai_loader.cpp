#include "precompiled.h"
#include "tai_loader.h"

const char TaiLoader::taiFormat[] = "%"QUOTE( MAX_CSTRING_LEN )"s %*s %u, %"QUOTE( ATLAS_TYPE_LEN )"s %f, %f, %f, %f, %f";

TaiLoader::TaiLoader( const char * name ) : pBuffer(), pLine(), bufferSize(), linesCount()
{
  if( loadFileToBuffer( name, &pBuffer, &bufferSize ) < 0 ) {
    printf( "File %s not found or corrupted", name );
    //return -2;
    pBuffer = 0;
    bufferSize = 0;
    pLine = 0;
  }
  else {
    while( pLine && pLine < pBuffer + bufferSize - 1 && ++pLine ) {
      switch( *pLine ) {
        case '#':
          pLine = strchr( pLine, '\n' );
          continue;

        case '\n':
          continue;
      }
      pLine = strchr( pLine, '\n' );
      ++linesCount;
    }
    pLine = pBuffer - 1;
  }

}

TaiLoader::~TaiLoader()
{
  delete []pBuffer;
}

/**
    \return prawda jesli linia została prawidłowo odczytana,
            fałsz gdy osiągnięto np. koniec pliku
*/
bool TaiLoader::read( TaiLineData * tld )
{
  while( pLine && pLine < pBuffer + bufferSize - 1 && ++pLine ) {
    switch( *pLine ) {
      case '#':
        pLine = strchr( pLine, '\n' );
        continue;

      case '\n':
        continue;

      case EOF:
        return false;
    }

    int result = sscanf( pLine, taiFormat, tld->filename, &tld->atlas_idx,
                         tld->atlas_type, &tld->woffset, &tld->hoffset,
                         &tld->depth_offset, &tld->width, &tld->height );
    switch( result ) {
      case fieldsToReadCount://ok
        //usuwam zbedne przecinki na koncu
        tld->atlas_type[strlen( tld->atlas_type )-1] = 0;

        pLine = strchr( pLine, '\n' );
        return true;
        break;
      case EOF:
        return false;
      default:    //error
        return false;
        break;
    }// koniec switch

    pLine = strchr( pLine, '\n' );
    return true;
  }
  return false;
}

