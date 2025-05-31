/*******************************************************************
$CRT 14 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>CreateVideoFrame.cpp<< 15 Okt 2024  10:01:39 - (c) proDAD
*******************************************************************/
#include "stdafx.h"

#pragma region struct CommonVideoFrame
struct CommonVideoFrame : IVideoFrame
{
  CVideoFrameInfo mInfo{};
  UINT8*          mBuffer{};

#ifdef HAVE_OPENCV_UMAT
  cv::UMat        mUMat;
#endif

  ~CommonVideoFrame()
  {
    if(mBuffer != nullptr)
      delete [] mBuffer;
  }

  /* Release this instance */
  void Release() override
  {
    delete this;
  }

  /* Retrieves information about the video frame */
  HRESULT GetInfo(CVideoFrameInfo* pInfo) override
  {
    HRESULT hr = 0;
    if(pInfo == nullptr)
      return E_POINTER;
    *pInfo = mInfo;
    return hr;
  }
};
#pragma endregion

#pragma region Utils
#ifdef HAVE_OPENCV_UMAT
/**
 * @brief Maps an internal format_type to an OpenCV mat type.
 * @param format The internal pixel format.
 * @return The corresponding OpenCV type (e.g., CV_8U, CV_8UC3) or 0 if unsupported (and asserts).
 */
static int GetCvType(format_type format)
{
  int CvType = 0;
  switch(format)
  {
    default:
      assert(0);
      break;

    case FMT_Gray8:
      CvType = CV_8U;
      break;

      // VUY8 is treated as a 3-channel 8-bit format for OpenCV processing.
    case FMT_VUY8:
      CvType = CV_8UC3;
      break;

    case FMT_BGR8:
      CvType = CV_8UC3;
      break;
  }
  return CvType;
}
#endif
#pragma endregion

#pragma region CreateVideoFrame
HRESULT CreateVideoFrame(
  IVideoFrame** ppFrame,
  SINT32 width,
  SINT32 height,
  FLOAT32 par,
  UINT32 format,
  FrameType_Enum frameType)
{
  HRESULT hr = 0;
  assert(ppFrame != nullptr && *ppFrame == nullptr);
  assert(width > 0 && height > 0);
  CommonVideoFrame* Frame = nullptr;
  UINT8* Buffer = nullptr;
  SINT32 Pitch;
  size_t BufSize;

  switch(format)
  {
    default:
      fprintf(stderr, "CreateVideoFrame: Unsupported Format %d\n", format);
      return E_FAIL;

    case FMT_Gray8:
      Pitch = width*1;
      Pitch = (Pitch+3) & ~3; // Align pitch to 4-byte boundary
      break;

    case FMT_BGR8:
    case FMT_VUY8:
      Pitch = width*3;
      Pitch = (Pitch+3) & ~3; // Align pitch to 4-byte boundary
      break;

    case FMT_BGRA8:
    case FMT_ARGB8:
      Pitch = width*4;
      // Pitch = (Pitch+3) & ~3; // This line seems to be missing the alignment for 4-byte formats.
                                 // If width*4 is already a multiple of 4, it's not strictly needed,
                                 // but for consistency with other cases, it might be intended.
                                 // However, if width*4 is always aligned, this is fine.
                                 // Let's assume width*4 is inherently 4-byte aligned.
      break;
  }

  BufSize = Pitch * height;

  Frame = new CommonVideoFrame;

  Frame->mInfo.Width  = width;
  Frame->mInfo.Height = height;
  Frame->mInfo.Format = format;
  Frame->mInfo.PAR    = par;

#ifdef HAVE_OPENCV_UMAT
  if(frameType == FrameType_CL)
  {
    Frame->mUMat.create(height, width, GetCvType((format_type)format));
    Frame->mInfo.pUMat = &Frame->mUMat;
  }
  else
#endif
  {
    (void)frameType; // Suppress unused variable warning for non-OpenCV builds
    Buffer = new UINT8[BufSize];
    if(!Buffer)
    {
      delete Frame;         // Clean up allocated Frame before returning.
      return E_OUTOFMEMORY;
    }

    Frame->mBuffer = Buffer;
    Frame->mInfo.Plane[0].Data = Buffer;
    Frame->mInfo.Plane[0].Pitch = Pitch;
  }

  *ppFrame = Frame;
  return hr;
}
#pragma endregion
