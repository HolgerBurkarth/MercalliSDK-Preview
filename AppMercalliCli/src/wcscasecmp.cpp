/*******************************************************************
$CRT 25 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>wcscasecmp.cpp<< 25 Jul 2014  16:39:05 - (c) proDAD
*******************************************************************/
#include "stdafx.h"
#include <wchar.h>
#include <wctype.h>

int wcscasecmp(const wchar_t* s1, const wchar_t* s2)
{
  wchar_t c1, c2;

  for(; *s1; s1++, s2++)
  {
    c1 = towlower(*s1);
    c2 = towlower(*s2);

    if(c1 != c2)
      return ((int)c1 - c2);
  }
  return (-*s2);
}