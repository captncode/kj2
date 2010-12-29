#include "parser.h"

Parser parser;

namespace parserNs {

void parserHelp();

Parser::Parser()
{
  addFun2( setParserVar, "set" );
  addFun0( parserHelp, "help" );
}

Parser::~Parser()
{
  for( __typeof( funcItem.begin() ) it = funcItem.begin();
       it != funcItem.end(); ++it )
  {
    delete it->second;
  }

  for( __typeof( varItem.begin() ) it = varItem.begin();
       it != varItem.end(); ++it )
  {
    delete it->second;
  }
}

int32_t Parser::explode( char * what, std::vector<char *>& w2 ) const
{
  std::vector<char * > word;
  std::vector<bool>    isQuote;
  word.reserve( 8 );
  writer::puts( "strtok begin" );
  //rozbijam względem cudzysłowów
  char * pch = strtok( what, "\"" );
  bool firstQuote = pch == what;
  while( pch )
  {
    word.push_back( pch );
    isQuote.push_back( firstQuote );
    firstQuote = !firstQuote;
    pch = strtok( 0, "\"" );
  }
  if( word.size() == 0 )
    return -1;
  //std::vector<char*> w2;
  w2.reserve( word.size() );
  //tam gdzie nie ma cudzysłowa rozbijam względem spacji
  for( int i = 0; i < ( int )word.size(); ++i ) {
    if( isQuote[i] ) {
      pch = strtok( word[i], " " );
      while( pch ) {
        w2.push_back( pch );
        writer::puts( pch );
        pch = strtok( 0, " " );
      }
    }
    else {
      w2.push_back( word[i] );
      writer::puts( word[i] );
    }
  }/*koniec for (i)*/
  writer::puts( "strtok end" );
  return 0;
}

//wektor wyjściowy trzyma indeksy kolenjnych słów
int32_t Parser::explode( char * what, std::vector<int>& w2 ) const
{
  std::vector<char * > word;
  std::vector<bool>    isQuote;
  word.reserve( 8 );
  writer::puts( "strtok begin" );
  //rozbijam względem cudzysłowów
  char * pch = strtok( what, "\"" );
  bool firstQuote = pch == what;
  while( pch )
  {
    word.push_back( pch );
    isQuote.push_back( firstQuote );
    firstQuote = !firstQuote;
    pch = strtok( 0, "\"" );
  }
  if( word.size() == 0 )
    return -1;
  //std::vector<char*> w2;
  w2.reserve( word.size() );
  //tam gdzie nie ma cudzysłowa rozbijam względem spacji
  for( int i = 0; i < ( int )word.size(); ++i ) {
    if( isQuote[i] ) {
      pch = strtok( word[i], " " );
      while( pch ) {
        w2.push_back( pch - what );
        writer::puts( pch );
        pch = strtok( 0, " " );
      }
    }
    else {
      w2.push_back( word[i] - what );
      writer::puts( word[i] );
    }
  }/*koniec for (i)*/
  writer::puts( "strtok end" );
  return 0;
}

int32_t Parser::explode( char * what, std::vector<std::string>& w2 ) const
{
  std::vector<char * > word;
  std::vector<bool>    isQuote;
  word.reserve( 8 );
  writer::puts( "strtok begin" );
  //rozbijam względem cudzysłowów
  char * pch = strtok( what, "\"" );
  bool firstQuote = pch == what;
  while( pch )
  {
    word.push_back( pch );
    isQuote.push_back( firstQuote );
    firstQuote = !firstQuote;
    pch = strtok( 0, "\"" );
  }
  if( word.size() == 0 )
    return -1;
  //std::vector<char*> w2;
  w2.reserve( word.size() );
  //tam gdzie nie ma cudzysłowa rozbijam względem spacji
  for( int i = 0; i < ( int )word.size(); ++i ) {
    if( isQuote[i] ) {
      pch = strtok( word[i], " " );
      while( pch ) {
        w2.push_back( pch );
        writer::puts( pch );
        pch = strtok( 0, " " );
      }
    }
    else {
      w2.push_back( word[i] );
      writer::puts( word[i] );
    }
  }/*koniec for (i)*/
  writer::puts( "strtok end" );
  return 0;
}

void Parser::objectNameToPointer( std::vector<char *>& w2 )
{
  for( int i = 1; i < ( int )w2.size(); ++i )
  { //zamieniam napisy w '' na adresy
    if( w2[i] && *w2[i] == '\'' )
    {
      ++w2[i];
      char * q = strchr( w2[i], '\'' );
      if( !q )  return;
      *q = 0;
      for( __typeof( objectItem.begin() ) it = objectItem.begin();
           it != objectItem.end(); ++it ) {
        if( strcmp( w2[i], it->first.c_str() ) == 0 )
        {
          BaseParserItem * bpi = it->second;
          void * ptr = bpi->getPointer<void *>();
          if( ptr )
          {
            //char tmp[12] = {};
            char * tmp = getBuffer( i, 12 );
            sprintf( tmp, "%p", ptr );
            writer::puts( tmp );
            w2[i] = tmp;
          }//if ptr
        }
      }/*koniec for(objectItem)*/
    }
  }
  return ;
}

void Parser::objectNameToPointer( std::vector<std::string>& w2 )
{
  for( int i = 1; i < ( int )w2.size(); ++i )
  { //zamieniam napisy w '' na adresy
    std::string & s = w2[i];
    if( s.c_str() && s.c_str()[0] == '\'' )
    {
      //s.erase(0,1);
      char * q = strchr( s.c_str() + 1, '\'' );
      if( !q )  return;
      *q = 0;
      for( __typeof( objectItem.begin() ) it = objectItem.begin();
           it != objectItem.end(); ++it ) {
        if( strcmp( s.c_str() + 1 , it->first.c_str() ) == 0 )
        {
          BaseParserItem * bpi = it->second;
          void * ptr = bpi->getPointer<void *>();
          if( ptr )
          {
            char tmp[12] = {};
            sprintf( tmp, "%p", ptr );
            writer::puts( tmp );
            s = tmp;
          }//if ptr
        }
      }/*koniec for(objectItem)*/
    }
  }
  return ;
}

void Parser::parse( char * what )
{
  //std::string what(what2);

  std::vector<char *> w2;
  if( explode( what, w2 ) < 0 )
    return;

  ItemIterator it = funcItem.find( what );
  if( it != funcItem.end() )
  {
    BaseParserItem * funcItem = it->second;
    objectNameToPointer( w2 );

    funcItem->execute( &w2[0], w2.size() );
    return;
  }

  char * dot = strtok( w2[0], "." );
  //size_t found = s.find_first_of('.');
  if( dot )
  { //pewnie metoda
    //writer::puts(dot );
    writer::printf( "looking for object '%s': ", dot );
    ItemIterator it = objectItem.find( dot );
    if( it != objectItem.end() ) {
      writer::puts( " found" );
      dot = strtok( NULL, "." );
      writer::printf( "looking for method '%s': ", dot );
      if( dot )
      {
        writer::puts( "method found" );
        w2[0] = dot;
        BaseParserItem * bpi = it->second;

        objectNameToPointer( w2 );

        bpi->execute( &w2[0], w2.size() - 1 );
        return;
      }
      else {
        puts( "method not found" );
        return;
      }
    }/*if*/else {
      writer::puts( " not found" );
    }
  }

}
void Parser::printAll()
{
  puts( "Parser commands:" );
  for( __typeof( funcItem.begin() ) it = funcItem.begin(); it != funcItem.end();
       ++it )
  {
    puts( it->first.c_str() );
  }
  puts( "Parser variables:" );
  for( __typeof( varItem.begin() ) it = varItem.begin(); it != varItem.end(); ++it )
  {
    puts( it->first.c_str() );
  }
  puts( "Parser obiects and methods:" );
  for( __typeof( objectItem.begin() ) it = objectItem.begin();
       it != objectItem.end(); ++it ) {

    printf( "%s\n", it->first.c_str() );
    //puts(it->first.c_str() );

  }

}

BaseParserItem * Parser::getFunction( const char * name )
{
  ItemIterator it = funcItem.find( name );
  if( it == funcItem.end() )
    return 0;

  return it->second;
}

void setParserVar( const char * varName, const char * value )
{
  //w[0] - polecenie
  puts( varName );
  Parser::ItemIterator it = parser.varItem.find( std::string( varName ) );
  if( it == parser.varItem.end() ) {
    puts( "variable not found" );
    return;
  }
  const char * w[] = {varName, value, 0};
  BaseParserItem * varItem = it->second;
  varItem->execute( const_cast<char **>( w ), 2 );

}

void parserHelp()
{
  parser.printAll();
}

}//namespace parserNs
