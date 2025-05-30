/*******************************************************************
$CRT 14 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>Utils.cpp<< 16 Jul 2014  09:26:34 - (c) proDAD
*******************************************************************/
#include "stdafx.h"

#pragma region ToUNICHAR
/* Very naive conversion.
 * Don't use in own programs.
 */
HRESULT ToUNICHAR(const char* input, UNICHAR* output, int outChars)
{
  assert(input != NULL && output != NULL && outChars > 0);

  int i = 0;
  for(; i<outChars-1 && input[i]!=0; ++i)
    output[i] = input[i];
  output[i] = 0;
  return 0;
}
#pragma endregion

#pragma region FrameTypeText
const char* FrameTypeText(FrameType_Enum v)
{
  switch(v)
  {
    default:
      assert(0);
      return "?";
    case FrameType_Standard:
      return "standard";
    case FrameType_CPU:
      return "cpu";
    case FrameType_CL:
      return "cl";
  }
}
#pragma endregion

