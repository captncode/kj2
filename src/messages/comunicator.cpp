#include "comunicator.h"

namespace ctr
{

//tworzy odrazu nullComunicator na pozycji o indexie 0
Comunicator Comunicator::nullComunicator;
std::vector<Comunicator::Comunicator_ptr> Comunicator::comunicator( 1, &Comunicator::nullComunicator );

bool Comunicator::sendNext( Message & msg ) const
{
  //comunicator[0] to zazwyczaj nullComunicator
  msg.changed = 0;
  const size_t comunicatorSize = comunicator.size();
  while( ++msg.lastComm < comunicatorSize ) {
    comunicator[msg.lastComm] -> replyRT( msg, this );
    if( msg.changed & 1 )   //1 to maska ustawiona przez set
      break;      //ktos zmienił dane komunikatu, warto aby user na to zareagował
  }
  if( msg.lastComm >= comunicatorSize ) {
    msg.reuse();
    return false;   // nie wywoływać juz więcej sendNext - komunikat został wysłany do wszystkich
  }
  return true; //msg.lastComm < comunicatorSize;  //tak, mozna jeszcze wywołać sendNext,
  //bo komunikat nie dotarł do wszystkich
}

}//namespace ctr
