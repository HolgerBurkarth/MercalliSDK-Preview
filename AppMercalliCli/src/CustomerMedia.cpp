/*******************************************************************
$CRT 22 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>CustomerMedia.cpp<< 23 Mai 2025  08:49:48 - (c) proDAD
*******************************************************************/
#include "stdafx.h"

#pragma region CustomerDecodeMediaService
struct CustomerDecodeMediaService : IDecodeMediaService
{
  HRESULT OpenVideoDecoder(IVideoDecoder** ppVideoDecoder, const char* fileName, UINT32 flags) override
  {
    const char* Txt = 
"Video Encoding and Decoding are not part of the SDK.\n"
"Before the sample itself can be used Media-Decoder must be implemented.";

    // TODO: Implement Media Decoder
    (void)ppVideoDecoder; // Suppress unused variable warning
    (void)fileName;      // Suppress unused variable warning
    (void)flags;        // Suppress unused variable warning

    #ifdef OS_WINDOWS
    MessageBoxA(NULL, Txt, "Mercalli SDK", MB_OK | MB_ICONEXCLAMATION);
    #else
    (void)Txt; // Suppress unused variable warning
    assert(0);
    #endif
    return E_FAIL;
  }
};
#pragma endregion

#pragma region CustomerEncodeMediaService
struct CustomerEncodeMediaService : IEncodeMediaService
{
  HRESULT CreateVideoEncoder(IVideoEncoder** ppVideoEncoder, const CMediaInfo& info, const char* fileName) override
  {
    const char* Txt =
"Video Encoding and Decoding are not part of the SDK.\n"
"Before the sample itself can be used Media-Encoder must be implemented.";

    // TODO: Implement Media Encoder
    (void)ppVideoEncoder; // Suppress unused variable warning
    (void)info;          // Suppress unused variable warning
    (void)fileName;      // Suppress unused variable warning

    #ifdef OS_WINDOWS
    MessageBoxA(NULL, Txt, "Mercalli SDK", MB_OK | MB_ICONEXCLAMATION);
    #else
    (void)Txt; // Suppress unused variable warning
    assert(0);
    #endif
    return E_FAIL;
  }
};
#pragma endregion

#pragma region Serv
CustomerDecodeMediaService gbCustomerDecodeMediaService;
CustomerEncodeMediaService gbCustomerEncodeMediaService;


IDecodeMediaService* DecodeMediaService = &gbCustomerDecodeMediaService;
IEncodeMediaService* EncodeMediaService = &gbCustomerEncodeMediaService;
#pragma endregion
