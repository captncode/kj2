#ifndef USEFUL_MACROS_H_INCLUDED
#define USEFUL_MACROS_H_INCLUDED

//zrodło: http://stackoverflow.com/questions/3385515/static-assert-in-c
#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1]
// token pasting madness:
#define COMPILE_TIME_ASSERT3(X,L) STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X,L) COMPILE_TIME_ASSERT3(X,L)
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

#include <assert.h>
extern "C" _CRTIMP int __cdecl __MINGW_NOTHROW  puts( const char* );
#define assert_puts_msg( x , msg ) if(! (x) ) puts(msg); assert(msg && x)

#ifndef QUOTE
#define _QUOTE(x) #x
#define QUOTE(x)   _QUOTE(x)
#endif /*QUOTE*/



#endif // USEFUL_MACROS_H_INCLUDED
