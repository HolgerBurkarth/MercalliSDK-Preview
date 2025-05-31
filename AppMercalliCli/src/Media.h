#pragma region Prolog
#ifndef __MERCALLICLI_MEDIA_H
#define __MERCALLICLI_MEDIA_H
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>Media.h<< 27 Feb 2020  14:43:39 - (c) proDAD
*******************************************************************/

/*! \warning Not part of the Mercalli SDK.
 * For use with the MercalliCli program only.
 */
#pragma endregion

#pragma region +++ Flowchart +++
 /*!
=========================================================================
 =                    Decode and Encode Frames                           =
 =========================================================================

 +------------------------+   +----------------+   +---------------------+
 |  OpenVideoDecoder      |-->|  CMediaInfo    |-->| CreateVideoEncoder  |
 +----------+-------------+   +----------------+   +---------+-----------+
            |               .                    .           |
            v               .                    .           v
   /---------------------/  .                    .   /-------------------/
  /   IVideoDecoder     /   .                    .  /   IVideoEncoder   /
 /---------------------/    .                    . /-------------------/
                            .                    .
                            .                    .
                            .                    .
 +------------------------+ .                    . +---------------------+
 |      ReadFrame         | .                    . |     WriteFrame      |
 +----------+-------------+ .                    . +---------------------+
            |               .                    .           ^
            v               .                    .           |
   /----------------------/ .                    .           |
  /     IVideoFrame      /  .                    .           |
 /----------+-----------/   .                    .           |
            |               .                    .           |
            +---------------------->-------------------------+
. . . . . . . . . . . . . . .                    . . . . . . . . . . . . .





 =========================================================================
 =                    Decode and Process Frames                          =
 =========================================================================

  +------------------------+
  |  OpenVideoDecoder      |
  +-----------+------------+
              |
              v
   /------------------------/   +------------------+
  /    IVideoDecoder       /--->|   CMediaInfo     |
 /------------------------/     +------------------+

  . . . . . . . . . . . . . . . . . . . . . . . . . .


          +------------------------+
          |      ReadFrame         |
          +-----------+------------+
                      |
                      v
            /----------------------/    +-------------------+
           /     IVideoFrame      / --->| CVideoFrameInfo   |
          /----------------------/      +--------+----------+
                                                 |
                                                 v
                                         /-------------------/
                                        /  Process Frame    /
                                       /---------+---------/
                                                 v
*/
#pragma endregion

#pragma region FrameType_Enum
/*! \brief Frame type enumeration for specifying frame processing methods.
 */
enum FrameType_Enum
{
  FrameType_Standard, /*!< Standard frame processing */
  FrameType_CPU,      /*!< CPU-based frame processing */
  FrameType_CL,       /*!< OpenCL-based frame processing */
};
#pragma endregion


#pragma region DecoderFlags_Enum
/*! \brief Decoder flags for configuring video decoding behavior.
 */
enum DecoderFlags_Enum
{
  DecF_Non = 0,             /*!< No special flags */
  DecF_SwapFields = 0x0001, /*!< Swap field order for interlaced video */
};
#pragma endregion

#pragma region CMediaInfo
/*! \brief Structure containing metadata about a video.
 */
struct CMediaInfo
{
  SINT32      Width{};                /*!< Width of the video in pixels */
  SINT32      Height{};               /*!< Height of the video in pixels */
  FLOAT32     Rate{30};               /*!< Frame rate (e.g., 29.97 for NTSC) */
  FLOAT32     PAR{1};                 /*!< Pixel aspect ratio (e.g., 1.067 for PAL, 1.333 for HDV) */
  field_type  FieldOrder{ FLD_Full }; /*!< Field order for interlaced videos */
  SINT32      FrameCount{};           /*!< Total number of frames in the video */
};
#pragma endregion

#pragma region CVideoFrameInfo
/*! \brief Structure containing information about a video frame, derived from Bitmap_Type.
 */
struct CVideoFrameInfo : Bitmap_Type
{
};
#pragma endregion

#pragma region IVideoFrame
/*! \brief Interface for handling video frames.
 */
struct IVideoFrame
{
  virtual ~IVideoFrame() = default; /*!< Default destructor */

  /*! \brief Releases the video frame instance.
   */
  virtual void Release() = 0;

  /*! \brief Retrieves information about the video frame.
   * \param pInfo Output parameter for frame information
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT GetInfo(
    /* out */ CVideoFrameInfo* pInfo) = 0;
};
#pragma endregion

#pragma region IVideoDecoder
/*! \brief Interface for decoding video frames.
 */
struct IVideoDecoder
{
  virtual ~IVideoDecoder() = default; /*!< Default destructor */

  /*! \brief Releases the video decoder instance.
   */
  virtual void Release() = 0;

  /*! \brief Retrieves metadata about the video.
   * \param pInfo Output parameter for video metadata
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT GetInfo(
    /* out */ CMediaInfo* pInfo) = 0;

  /*! \brief Reads a frame from the video.
   * \param ppFrame     Output parameter for the new frame (must be released)
   * \param frameNumber Frame index [0 ... CMediaInfo::FrameCount-1]
   * \param width       Required pixel width
   * \param height      Required pixel height
   * \param format      Required pixel format (format_type)
   * \param frameType   Frame processing type (default: FrameType_Standard)
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT ReadFrame(
    /* out */ IVideoFrame** ppFrame,
    /* in  */ SINT64 frameNumber,
    /* in  */ SINT32 width,
    /* in  */ SINT32 height,
    /* in  */ format_type format,
    /* in  */ FrameType_Enum frameType = FrameType_Standard
  ) = 0;
};
#pragma endregion

#pragma region IVideoEncoder
/*! \brief Interface for encoding video frames.
 */
struct IVideoEncoder
{
  virtual ~IVideoEncoder() = default; /*!< Default destructor */

  /*! \brief Releases the video encoder instance.
   */
  virtual void Release() = 0;

  /*! \brief Writes a frame to the encoder stream.
   * \param pFrame Input frame to encode
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT WriteFrame(
    /* in  */ IVideoFrame* pFrame) = 0;

  /*! \brief Creates a target frame for encoding.
   * \param ppFrame Output parameter for the new frame (must be released)
   * \param format  Required pixel format (format_type)
   * \param frameType Frame processing type (default: FrameType_Standard)
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT CreateVideoFrame(
    /* out */ IVideoFrame** ppFrame,
    /* in  */ format_type format,
    /* in  */ FrameType_Enum frameType = FrameType_Standard
  ) = 0;
};
#pragma endregion

#pragma region IDecodeMediaService
/*! \brief Interface for opening and decoding media files.
 */
struct IDecodeMediaService
{
  virtual ~IDecodeMediaService() = default; /*!< Default destructor */

  /*! \brief Opens a media file for decoding.
   * \param ppVideoDecoder Output parameter for the new decoder (must be released)
   * \param fileName       Path to the media file (e.g., "test.mp4")
   * \param flags          Decoder configuration flags (DecoderFlags_Enum)
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT OpenVideoDecoder(
    /* out */ IVideoDecoder** ppVideoDecoder,
    /* in  */ const char* fileName,
    /* in  */ UINT32           flags) = 0;
};
#pragma endregion

#pragma region IEncodeMediaService
/*! \brief Interface for creating and managing video encoders.
 */
struct IEncodeMediaService
{
  virtual ~IEncodeMediaService() = default; /*!< Default destructor */

  /*! \brief Creates a video encoder.
   * \param ppVideoEncoder Output parameter for the new encoder (must be released)
   * \param info           Video metadata (CMediaInfo)
   * \param fileName       Path to the output media file (e.g., "test.mp4")
   * \return HRESULT indicating success or failure
   */
  virtual HRESULT CreateVideoEncoder(
    /* out */ IVideoEncoder** ppVideoEncoder,
    /* in  */ const CMediaInfo& info,
    /* in  */ const char* fileName) = 0;
};
#pragma endregion

#pragma region Functions

/*! \brief Media service interfaces for decoding and encoding.
 */                         
extern IDecodeMediaService* DecodeMediaService;
extern IEncodeMediaService* EncodeMediaService;

/*! \brief Opens a media file for decoding.
 * \param ppVideoDecoder Output parameter for the new decoder (must be released)
 * \param fileName       Path to the media file (e.g., "test.mp4")
 * \param flags          Decoder configuration flags (DecoderFlags_Enum)
 * \return HRESULT indicating success or failure
 */
inline HRESULT OpenVideoDecoder(
  /* out */       IVideoDecoder** ppVideoDecoder,
  /* in  */ const char*           fileName,
  /* in  */ UINT32                flags)
{
  assert(ppVideoDecoder != NULL && fileName != NULL && *ppVideoDecoder == NULL);
  assert(DecodeMediaService != NULL); /*!< Ensures the decode service is defined */
  return DecodeMediaService->OpenVideoDecoder(ppVideoDecoder, fileName, flags);
}

/*! \brief Creates a video encoder.
 * \param ppVideoEncoder Output parameter for the new encoder (must be released)
 * \param info           Video metadata (CMediaInfo)
 * \param fileName       Path to the output media file (e.g., "test.mp4")
 * \return HRESULT indicating success or failure
 */
inline HRESULT CreateVideoEncoder(
  /* out */       IVideoEncoder** ppVideoEncoder,
  /* in  */ const CMediaInfo&     info,
  /* in  */ const char*           fileName)
{
  assert(ppVideoEncoder != NULL && fileName != NULL && *ppVideoEncoder == NULL);
  assert(EncodeMediaService != NULL); /*!< Ensures the encode service is defined */
  return EncodeMediaService->CreateVideoEncoder(ppVideoEncoder, info, fileName);
}


/*! \brief Creates a frame for use as a frame buffer.
 * \param ppFrame   Output parameter for the new frame (must be released)
 * \param width     Frame width in pixels
 * \param height    Frame height in pixels
 * \param par       Pixel aspect ratio
 * \param format    Pixel format (format_type)
 * \param frameType Frame processing type (default: FrameType_Standard)
 * \return HRESULT indicating success or failure
 */
HRESULT CreateVideoFrame(
  /* out */ IVideoFrame** ppFrame,
  /* in  */ SINT32  width,
  /* in  */ SINT32  height,
  /* in  */ FLOAT32 par,  /*!< Pixel aspect ratio */
  /* in  */ UINT32  format, /*!< Pixel format (enum format_type) */
  /* in  */ FrameType_Enum frameType = FrameType_Standard

);


#pragma endregion


#pragma region Epilog
#endif // __MERCALLICLI_MEDIA_H
#pragma endregion
