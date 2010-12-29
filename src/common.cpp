#include "common.h"

/**
    Należy samemu zwolnic pBuffer
    \param name nazwa pliku
    \param [out] pBuffer wczytane dane
    \param [out] dlugosc tablicy spod pBuffer, dlugość pliku
    \return kod błędu
*/
int32_t loadFileToBuffer( const char * name, char ** pBuffer, size_t * pOutSize )
{
  FILE * pFile = fopen( name, "r" );
  if( !pFile )
    return -1;
  fseek( pFile, 0, SEEK_END );
  const uint32_t fileSize = ftell( pFile );
  fseek( pFile, 0, SEEK_SET );

  char * buffer = new char[fileSize+1];
  //memset(buffer,0,fileSize+1);
  size_t result = fread( buffer, 1, fileSize, pFile );
  buffer[result] = 0;
  //if(result != fileSize){
  //result jest rózny od fileSize bo fileSize to rozmiar w bajtach
  //result to ilość poprawnie odczytanych elementów
  //a przy kodowaniu windowsowym, nawet w ASCII to może sie różnić
  //(LF+CR zamiast LF)
  //}

  fclose( pFile );

  *pBuffer = buffer;
  //*pOutSize = fileSize;
  *pOutSize = result;
  return fileSize;

}
