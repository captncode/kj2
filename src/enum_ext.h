#pragma once
#include <cstring>

namespace impl
{


template <int I>
int tokenizeEnum( char * str )
{
  //static const char tokens[] = " ,=1234567890";
  //puts(str);
  static const char tokens[] = ", ";
  char out[I] = {};
  char * ext = strtok( str, tokens );
  size_t offset = 0, count = 0;
  while( ext != NULL ) {
    ++count;
    const size_t strl = strlen( ext ) + 1 ;
    memcpy( out + offset, ext, strl );
    offset += strl;
    out[offset-1] = ' ';
    ext = strtok( NULL, tokens );
  }
  if( count ) {
    memcpy( str, out, I );
    str[I] = 0;
  }
  return count;
}
/*
typedef std::vector<char*> vecPch;

  template <int I>
  const vecPch tokenizeEnumVEC(char* str){
    //static const char tokens[] = " ,=1234567890";
    static const char tokens[] = ", ";
        char out[I] = {};
    char* ext = strtok (str,tokens);
    size_t offset = 0,count = 0;
    vecPch vNames;
        while (ext != NULL)
        {
      const size_t strl = strlen(ext) +1 ;
            memcpy(out + offset,ext,strl);
      vNames.push_back(str + offset);
      offset += strl;
      //out[offset-1] = ' ';
            ext = strtok (NULL,tokens);
      ++count;
        }
    if(count){
      memcpy(str,out,I);
      str[I] = 0;
    }
    return vNames;
  }
*/
#ifdef UNCOMMENT
template <int I>
const char ** tokenizeEnumEx( char * str )
{
  static const char tokens[] = " ,=0123456789";
  char out[I] = {};   //< miejsce na przekopiowanie napisu
  char * ext = strtok( str, tokens ); //< rozbicie na tokeny
  size_t offset = 0, count = 0;
  char ** vNames = new char*[300]; // tymczasowy bufor, mozna zast¹piæ vectorem
  while( ext != NULL ) {
    const size_t strl = strlen( ext ) + 1;
    if( 1 < strl ) {
      memcpy( out + offset, ext, strl );  //< kopiuje token do bufora
      vNames[count] = ( str + offset );   //< ustawiam wyjsciowy wska¿nik
      offset += strl;
    }
    ext = strtok( NULL, tokens );
    ++count;
  }
  if( count ) {
    memcpy( str, out, I );  //< kopiuje ca³y napis do wyjsciowego bufora
    char ** out = new char*[count+1]; // zeby tablica niebyla za duza to alokuje now¹
    out[count] = 0;

    memcpy( out, vNames, count * sizeof( char * ) );
    delete [] vNames;
    return ( const char ** )out;
  }
  delete [] vNames;
  return 0;
}

template <int I>
const char ** tokenizeEnumValueEx( char * str )
{
  static const char tokens[] = ",";
  char out[I] = {};
  char * ext = strtok( str, tokens );
  size_t offset = 0, count = 0;
  char ** vNames = new char*[300]; // tymczasowy bufor
  while( ext != NULL ) {
    const size_t strl = strlen( ext ) + 1;
    if( 1 < strl ) {
      //printf("\next: %s\n",ext+1);
      memcpy( out + offset, ext, strl );
      //*(out + offset + strl) = 0;
      vNames[count] = ( str + offset );
      offset += strl;
    }
    ext = strtok( NULL, tokens );
    if( ext ) ext++; //wywalam spacje
    ++count;
  }
  //printf("count:%u \n",count);
  if( count ) {

    memcpy( str, out, I );
    //str[I] = 0;
    char ** out = new char*[count+1];
    out[count] = 0;

    memcpy( out, vNames, count * sizeof( char * ) );
    delete [] vNames;
    return ( const char ** )out;
  }
  delete [] vNames;
  return 0;
}
#endif //UNCOMMENT*/


//! wersje pierwszą zostawiłem dla porównania :P
//! różnica jest masakryczna, a mineło raptem pare miesięcy,
//! no ale tamto pisałem pózno w nocy
template <int I>
const char ** tokenizeEnumEx2( char( &str )[I], const char * tokens = " ,=0123456789" )
{
  typedef char * PChar;
  //static const char tokens[] = " ,=0123456789";
  static char * out[I] = {}; //! wszystkie wskażniki na 0

  int count = 0;
  char * ext = strtok( str, tokens ); //< dostaje wskażnik do pierwszego tokena
  while( ext != NULL ) {
    const size_t lenExt = strlen( ext ) + 1;
    if( lenExt > 1 ) {   //! to samo co: strlen(ext) > 0
      //! zeruje końcówke tego napisu(pewnie siedzi tam jakaś wartość z tokens)
      //! tak aby była null-terminated :P
      ext[lenExt-1] = 0;
      int firstNonWhite = strspn( ext, " \n\t" );
      out[count] = ext + firstNonWhite;
    }
    ext = strtok( NULL, tokens );
    ++count;
  }
  return const_cast<const char ** >( out );
}



}//namespace impl
#define BIG_ENUM(name,...) \
  enum name{ \
    __VA_ARGS__ \
  }; \
  char name##_names[] = {#__VA_ARGS__}; \
  int tmp_sciema##name##a = impl::tokenizeEnum<sizeof(name##_names)>(name##_names)

#define ENUM_VEC(name,...) \
  namespace name { \
  char name##_all_names[] = {#__VA_ARGS__}; \
  const vecPch& r_namesVec = impl::tokenizeEnumVEC<sizeof(name##_all_names)>(name##_all_names); \
  vecPch name(r_namesVec.begin(),r_namesVec.end() ); \
  enum{ \
    __VA_ARGS__  \
  }; \
  }

#define ENUM_EX_POOR(name,...) \
  char name##_all_names[] = {#__VA_ARGS__}; \
  const char** name = impl::tokenizeEnumEx<sizeof(name##_all_names)>( name##_all_names ); \
  enum name##_{ \
    __VA_ARGS__  \
  }


#define ENUM_CLASS_EX(name,...) \
  char name##_all_names[] = {#__VA_ARGS__}; \
  struct name{ \
    enum name{ \
      __VA_ARGS__ \
    } en;\
    static const char* asText(int index){\
      static const char** names = impl::tokenizeEnumEx<sizeof(name##_all_names)>( name##_all_names ); \
      return names[index];\
    }\
    name(enum name e):en(e){}\
    const name& operator=(int e){ en = e ; return *this;}; \
    operator int(){ return en; }\
  }


//! działa, sprawdzone,ale funkcje nie sa w namespace wiec sie jebie
//! odradzane :P
#define ENUM_EX_NON_NS(name,...) \
  enum E##name{ \
    __VA_ARGS__ \
  };\
  namespace name{\
  \
  inline const char** getStaticNameArr(){\
    static char name##_all_names[] = {#__VA_ARGS__}; \
    static const char** names = ::impl::tokenizeEnumEx2( name##_all_names ); \
    return names;\
  }\
  inline const char** getStaticNameAndValueArr(){\
    static char name##_all_namesValues[] = {#__VA_ARGS__}; \
    static const char** names = ::impl::tokenizeEnumEx2( name##_all_namesValues, "," ); \
    return names;\
  }\
  \
  inline const char* asText(int index){\
    return getStaticNameArr()[index];\
  }\
  template <int I> inline const char* asText(){\
    return getStaticNameArr()[I];\
  }\
  \
  inline const char* asTextWithVal(int index){\
    return getStaticNameAndValueArr()[index];\
  }\
  template <int I> inline const char* asTextWithVal(){\
    return getStaticNameAndValueArr()[I];\
  }\
  }

//! działa, sprawdzone
#define ENUM_EX(name,...) \
  namespace name{\
  enum name{ \
    __VA_ARGS__ \
  };\
  \
  inline const char** getStaticNameArr(){\
    static char name##_all_names[] = {#__VA_ARGS__}; \
    static const char** names = ::impl::tokenizeEnumEx2( name##_all_names ); \
    return names;\
  }\
  inline const char** getStaticNameAndValueArr(){\
    static char name##_all_namesValues[] = {#__VA_ARGS__}; \
    static const char** names = ::impl::tokenizeEnumEx2( name##_all_namesValues, "," ); \
    return names;\
  }\
  \
  inline const char* asText(int index){\
    return getStaticNameArr()[index];\
  }\
  template <int I> inline const char* asText(){\
    return getStaticNameArr()[I];\
  }\
  \
  inline const char* asTextWithVal(int index){\
    return getStaticNameAndValueArr()[index];\
  }\
  template <int I> inline const char* asTextWithVal(){\
    return getStaticNameAndValueArr()[I];\
  }\
  }\
  typedef name::name E##name;

