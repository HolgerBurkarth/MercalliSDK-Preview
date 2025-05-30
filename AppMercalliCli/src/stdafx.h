#ifndef __MERCALLICLI_STDFX_H
#define __MERCALLICLI_STDFX_H
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>stdafx.h<< 24 Mai 2025  09:44:58 - (c) proDAD
*******************************************************************/
#include <proDADTypes.h>
#include <stdio.h>
#include <string.h>

/*
* The PRId64 macro is used to format 64-bit integers in printf-style functions.
* @example:
*  SINT64 value = 1234567890123456789;
* printf("Value: %" PRId64 "\n", value);
*/
#ifndef PRId64
  #ifdef _MSC_VER
    #define PRId64 "I64d"
  #else
    #define PRId64 "ld"
  #endif
#endif

/*
* The PRIws macro is used to format wide strings in printf-style functions.
* @example:
* const wchar_t* wideStr = L"Hello, World!";
* printf("Wide String: %" PRIws "\n", wideStr);
*/
#ifndef PRIws
  #ifdef _MSC_VER
    #define PRIws "ws"
  #else
    #define PRIws "ls"
  #endif
#endif


#if defined(OS_MACOS10) || defined(OS_LINUX)
  #include <wchar.h>

  #define stricmp  strcasecmp
  #define wcsicmp  wcscasecmp
  #define strnicmp strncasecmp
  #define wcsnicmp wcsncasecmp

#endif

#if defined(OS_WINDOWS)
  #undef max
  #undef min
#endif

/* --- proDAD internals */
#if defined(MEDULLA_LINK_DYNLIB) || defined(MEDULLA_LINK_STATICLIB)
  #include <proDADSystem.h>
  #include <MedullaMedia.h>
  #include <OpenCV2.h>
  #include <OcvPMat.h>
  #include "StaticLibInternals.h"
  using namespace proDAD;
  using namespace proDAD::pSystem;

  #define USE_GetPerformanceTime 1
#endif


#include "mercalli.h"
#include <algorithm> // std::max etc.

#include <Media.h>
#include "Utils.h"
#include "VideoStabilization.h"
#include "VideoUnjello.h"



#endif // __MERCALLICLI_STDFX_H
