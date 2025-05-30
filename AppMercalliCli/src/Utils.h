#ifndef __MERCALLICLI_UTILS_H
#define __MERCALLICLI_UTILS_H
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>Utils.h<< 22 Feb 2020  07:27:24 - (c) proDAD
*******************************************************************/

/* Very naive conversion.
 * Don't use in own programs.
 */
HRESULT ToUNICHAR(const char* input, UNICHAR* output, int outChars);

const char* FrameTypeText(FrameType_Enum v);


#pragma region Epilog
#endif // __MERCALLICLI_UTILS_H
#pragma endregion
