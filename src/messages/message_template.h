#ifndef _MESSAGE_TEMPLATE_H_GUARD
#define _MESSAGE_TEMPLATE_H_GUARD

#include "useful_macros.h"

#define MESSAGE_GET_ASSIGNED_TYPE(msg)          messageToTypeMap[msg*2+1]

/*! \struct Message
    szablony w razie podania złego typu sie nieskompilują, a tutaj dostaje błąd w runtime
    jak narazie zakładam ze wysyłane bedą lekkie obiekty
    gdy w gre wejdą cieższe to zamiast operatora== w metodach set i get używać się bedzie niestałej referencjji do
    wskażnika zamiast referencji
    w konstruktorze uzywam niestałej referencji, aby było jasne ze wynik zapytania bedzie w przekazanym obiekcjie
    Po co ta referencja w konstruktorze? Jesli chce wysłać jakieś dane to podaje je jako parametr,
    jeśli chce odebrać jakieś informacje to moge stworzyć na stosie zmienną i ją przekazac jako parametr.
    Message::last ustawiam na 0 zeby omijać nullComunicator
*/


#define MESSAGE_GENERATE_SYMBOL_FOR(TYPE) template<>    \
  struct TypeSymbolic<TYPE>{                          \
    enum{ TYPE_SYMBOL = __COUNTER__ };              \
  };

#define createMessage_(MSG, DATA_PTR) Message(MSG,DATA_PTR,             \
    QUOTE(__FILE__ in line __LINE__ : Wrong argument type!  \
          Check file: MESSAGE_USER_FILENAME ))

#define messageSet_(MSG_OBJECT, DATA_REF) MSG_OBJECT.set(DATA_REF,      \
    QUOTE(__FILE__ in line __LINE__ : Wrong argument type!  \
          Check file: MESSAGE_USER_FILENAME ))

#define messageGet_(MSG_OBJECT, DATA_REF) MSG_OBJECT.get(DATA_REF,      \
    QUOTE(__FILE__ in line __LINE__ : Wrong argument type!  \
          Check file: MESSAGE_USER_FILENAME ))

#define MESSAGE_DEFINE(NAME,TYPE_PTR)
#define MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH(comment)

#include QUOTE(MESSAGE_USER_FILENAME)

/*! \class TypeSymbolic
    uzycie makra counter jest potencjalnie mniej przenośne,
    ale dzieki temu jako TYPE akceptowany jest np. 'int*'
*/
template <class T> struct TypeSymbolic {};


#define ASSIGN_TEMPLATE_SYMBOL_TO_MESSAGE(MSG,TYPE) MSG,TypeSymbolic<TYPE>::TYPE_SYMBOL,

#undef MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH
#define MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH MESSAGE_GENERATE_SYMBOL_FOR

#include QUOTE(MESSAGE_USER_FILENAME)


enum MessageDefinition {
#undef MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH
#define MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH(comment)

#undef MESSAGE_DEFINE
#define MESSAGE_DEFINE(NAME,TYPE_PTR) NAME,
#include QUOTE(MESSAGE_USER_FILENAME)

};

static uint32_t messageToTypeMap[] = {
#undef MESSAGE_DEFINE
#define MESSAGE_DEFINE ASSIGN_TEMPLATE_SYMBOL_TO_MESSAGE
#include QUOTE(MESSAGE_USER_FILENAME)
};

struct Message {
  template < class T >
  Message( uint32_t msg_, T & ptr, const char * callLog ) :
    msg( msg_ ), type( TypeSymbolic<T>::TYPE_SYMBOL ), general_ptr( &ptr ), lastComm( 0 ), lastLisn( 0 ) {
    //COMPILE_TIME_ASSERT(TypeSymbolic<T>::TYPE_SYMBOL == MESSAGE_GET_ASSIGNED_TYPE(msg) );
    assert_puts_msg( TypeSymbolic<T>::TYPE_SYMBOL == MESSAGE_GET_ASSIGNED_TYPE( msg ), callLog );
  }
  template < class T >
  void set( const T & ref, const char * callLog ) {
    assert_puts_msg( TypeSymbolic<T>::TYPE_SYMBOL == MESSAGE_GET_ASSIGNED_TYPE( msg ), callLog );
    *( T * )general_ptr = ref;
    changed |= 1;
  }
  template < class T >
  void get( T & ref, const char * callLog ) const {
    assert_puts_msg( TypeSymbolic<T>::TYPE_SYMBOL == MESSAGE_GET_ASSIGNED_TYPE( msg ), callLog );
    ref = *( T * )general_ptr;
    changed |= 2;
  }

  operator uint32_t() const { return msg; }

private:
  void reuse() { lastComm = 0; lastLisn = 0; changed = 0; }

  union {
    void * general_ptr;
    char space [8];
  };

  uint16_t msg;   //max 16k rodzai wiadomości
  uint16_t type;  //max 16k rożnych typów
  //! do którego komunikatora zostanie wysłany ten komunikat w sendNext
  uint16_t lastComm;
  uint16_t lastLisn;           //!< do którego listenera zostanie posłany
  //! czy wartość w wyniku wysłania komunikatu została zmodyfikowana
  mutable uint8_t changed;   //!< używać jak flagi: &1 - set; &2 - get;

  friend class ctr::Comunicator;
  template<class> friend class ctr::ListenableComunicator;
};

#define MESAGE_FOOTER

#undef MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH
#define MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH(dupa)
#undef MESSAGE_DEFINE
#define MESSAGE_DEFINE(cycki,dupcia)
#include QUOTE(MESSAGE_USER_FILENAME)

#undef MESAGE_FOOTER

#undef MESSAGE_DEFINE
#undef MESSAGE_ADD_TYPE_OF_DATA_TO_SEND_WITH
#undef MESSAGE_DECLARE_POINTER
#undef MESSAGE_INTERFACE
#undef MESSAGE_SYMBOL_OF_PTR
#undef MESSAGE_SYMBOL_OF_PTR_COMMA
#undef MESSAGE_GET_ASSIGNED_TYPE
#undef ASSIGN_TYPE_PTR_TO_MESSAGE
#undef MESSAGE_INTERFACE_FULL_DEBUG
#undef MESSAGE_INTERFACE_NON_DEBUG

#endif /* _MESSAGE_TEMPLATE_H_GUARD */
