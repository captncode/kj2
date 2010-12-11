/*
 * precompiled.h
 *
 *  Created on: 2010-11-12
 *      Author:
 */

#ifndef PRECOMPILED_H_
#define PRECOMPILED_H_

#include <cstdio>
#include <stdint.h>
#include <cmath>
#include <algorithm>    //max
#include <map>
#include <new>    //placement new
#include <vector>
#include <string>
#include "stdint.h"   //stdint to glownie typedefy na typy wbudowane
#include <limits>

#include <Sdl/Sdl.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#define EXTERNFLAG extern
#include "external/gl3header.h"


//moje naglowki
#include "common.h"

#define _QUOTE(x) #x
#define QUOTE(x)   _QUOTE(x)
#define _LINE_STRING_   QUOTE(__LINE__)

#define ERROR_CODE      -1
#define OK_CODE         0
#define FRAME_OK              FRAME_OK_CODE

#ifdef _DEBUG
#define PRINT_ERROR(cstr) ( printf("error in: %s,\tline: %s\n: %s",\
                                   __PRETTY_FUNCTION__,_LINE_STRING_,cstr) )
#else   /*_DEBUG*/
#define PRINT_ERROR(unused) ERROR_CODE
#endif  /*_DEBUG*/

#define PRINTF_SDL_ERROR(text)    printf(text " %s\n", SDL_GetError());

#ifdef _DEBUG
#define DEBUG_PRINTF(variable,format)     printf(#variable ":\t" format,variable)
#else
#define DEBUG_PRINTF(variable,format)
#endif

#define FOR_ALL(c,it) for( __typeof(c.begin()) it = c.begin();it != c.end(); ++it )




#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif /*UCHAR_MAX*/


#endif /* PRECOMPILED_H_ */
