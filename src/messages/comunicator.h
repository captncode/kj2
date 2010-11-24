#pragma once

#include <algorithm>        //remove
#include <stdint.h>
#include <vector>
#include <assert.h>

#include "useful_macros.h"

/*! \def MESSAGE_USER_FILENAME
    Globsalna definicja napisu zawierającego nazwe pliku z którego zostanie
    wygenerowana structura Message
*/
/*! \def MESSAGE_FULL_DEBUG
  Włącza pełne debugowanie przesyłanych komunikatów. Dodaje do akomunikatu
  w razie niepowodzenia, gdzie wystąpiło błędne wywołanie.
  Assertów zaleca sie niewyłączać.
*/

#ifndef MESSAGE_USER_FILENAME
//# error Please define MESSAGE_USER_FILENAME with your filename, before including this file
#define MESSAGE_USER_FILENAME comunicator_blank_message.h
#endif

namespace ctr
{
class Comunicator;
template<class> class ListenableComunicator;
}//namespace ctr

#include "message_template.h"

namespace ctr
{

enum MessageDispatchStates {
  REPLY_OK,
  REPLY_IGNORED,
};

/*! \fn Comunicator::sendNext
    Wiadomość jest wysyłana aż któryś z komunikatorów ją obsłuży,
    lub aż osiągie koniec tablicy.
    Gdy ktoś obsłuży komunikat pętla jest przerywana, tak aby uźytkownik mógł
    obsłuźyć wynik komunikatu.
    \return czy są jeszcze komunikatory do których nie została wysłana ta wiadomość
*/

bool sSendAll( Message & msg );

//! \class Comunicator
/*! nadpisując replyRT należy wywołać wersje z klasy bazowej*/
class Comunicator
{
public:
  Comunicator() {
    comunicator.push_back( this );
  }
  virtual ~Comunicator() {
    std::remove( comunicator.begin() + 1, comunicator.end(), this );
    comunicator.pop_back();
  }

  inline bool sendAll( Message & msg ) const {
    //zaczynam od 1 - omijam nullComunicator
    for( int i = 1; i < ( int )comunicator.size(); ++i ) {
      comunicator[i]->replyRT( msg, this );
    }//koniec for (i)
    return 0;
  }

  /*! \return czy komunikat został wysłany do wszystkich zainteresowanych
  */
  virtual bool sendNext( Message & msg ) const ;

  inline virtual uint32_t replyRT( Message & msg, const Comunicator * sender ) {
    ( void )sizeof( msg );
    ( void )sizeof( sender );

    return REPLY_IGNORED;
  }

protected:
  typedef Comunicator * Comunicator_ptr;
  static Comunicator nullComunicator;
  static std::vector<Comunicator_ptr> comunicator;

  friend bool sSendAll( Message & msg );
};

//static send
inline bool sSendAll( Message & msg )
{
  ctr::Comunicator::comunicator[0]->sendAll( msg );
  return false;
}

//! \class ListenableComunicator - nie dziedziczyć po tej klasie bo można całe 'lisenable' rozjebać!
/*! */
template <class L>
class ListenableComunicator : public Comunicator
{

public:
  ListenableComunicator( L * l ) : listener( l ) {}
  ~ListenableComunicator() {}

  inline uint32_t replyRT( Message & msg, const Comunicator * sender ) {
    listener->gotMsg( msg, sender );
    return 0;
  }

  void setListener( L * l ) {
    listener = l;
  }

private:
  L * listener;
}; // koniec ListenableComunicator



}//namespace ctr
