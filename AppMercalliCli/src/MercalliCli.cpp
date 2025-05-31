#pragma region Prolog
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>MercalliCli.cpp<< 28 Mai 2025  07:53:29 - (c) proDAD
*******************************************************************/
#pragma endregion
#pragma region Test Params (proDAD)
/*
--source D:\E\c3\extern\MediaPool\clips\720HdNtsc-Vado-SchwenkKippDreh.MP4 --cl --target "d:\TEMP\t.mp4" --Profile Universal --pref ON --RS ON --out 3 --BorderMode "Raw" --TranslationSmoothing 1 --verbose ON --stabilize

--source "D:\TestClips\tmp\GOPR9643.MP4" --cl --target "d:\TEMP\t.mp4" --Profile AI --pref ON --in 165 --out 189 --BorderMode "Raw" --verbose ON --stabilize

--source "D:\TestClips\Mercalli6\720HdNtsc60Gopro6ComplexDroneRU.mp4" --cl --target "d:\TEMP\t.mp4" --Profile AI --pref ON --in 0 --out 20 --BorderMode "Raw" --verbose ON --stabilize

--source "D:\R\foreign\gfx2\anim1\HDV60i-Flugzeug.mpeg" --cl --target "d:\TEMP\t.mp4" --Profile AI --pref ON --BorderMode "Raw" --TranslationSmoothing 1 --verbose ON --stabilize

--source "D:\R\foreign\gfx2\anim1\hubi01-hd.mpeg" --cl --target "d:\TEMP\t.mp4" --Profile AI --pref ON --BorderMode "Raw" --TranslationSmoothing 1 --verbose ON --stabilize
*/
#pragma endregion
#pragma region Includes
#include "stdafx.h"
//#define _EXTEST_

#ifdef _EXTEST_
#include <proDADInternet.h>
#endif
#pragma endregion
#pragma region +++ Mercalli SDK +++
/* Mercalli SDK
 * This software development kit is provided under license from
 * proDAD GmbH and may only be used in accordance with that license.
 *
 * proDAD GmbH
 * Gauertstr. 2
 * D-78194 Immendingen (Germany)
 * For SDK technical support, please contact: Burkarth@prodad.com
 */
#pragma endregion
#pragma region +++ Flowchart +++
/*

 ===============================================================================
 =                      The three possible methods                             =
 ===============================================================================

           <1>                        <2>                        <3>
            |                          |                          |
=========================  =======================   ===========================
| Video Stabilization   |  | Video Unjello       |   | Remove CMOS + Stabilize |
| (Smooth Camera Path)  |  | Remove CMOS Effects |   | (Combi)                 |
=========================  =======================   ===========================
            |                          |                          |
            v                          v                          v
   /--------------------/    /--------------------/     /--------------------/
  /  Open Source Video /    /  Open Source Video /     /  Open Source Video /
 /--------------------/    /--------------------/     /--------------------/
            |                          |                          |
            v                          v                          v
+----------------------+   +---------------------+   +-------------------------+
|  Perform: <10><12>   |   |   Perform: <11><13> |   |      Perform: <11><13>  |
|  Video Stabilization |   |   Video Unjello     |   |      Video Unjello      |
+-----------+----------+   +---------------------+   +-------------------------+
            |                          |                          |
            v                          v                          v
  /--------------------/    /--------------------/      /--------------------/
 / Write Target Video /    / Write Target Video /      /  Write Temp. Video /
/--------------------/    /--------------------/      /--------------------/
                                                                  |
                                                                  v
                                                        /--------------------/
                                                       /  Open Temp. Video  /
                                                      /--------------------/
                                                                  |
                                                                  v
                                                    +--------------------------+
                                                    |    Perform: <10><12>     |
                                                    |    Video Stabilization   |
                                                    +--------------------------+
                                                                  |
                                                                  v
                                                        /--------------------/
                                                       / Write Target Video /
                                                      /--------------------/




 ===============================================================================
 =                           CMOS Video Process                                =
 ===============================================================================

           Start
             |
             v
    /----------------\ Yes   /----------------------\ Yes
    | if CMOS Video  |------>|   Are strong CMOS    |--->-----------+
    \--------+-------/       | distortions contain? |               |
             | No            \----------------------/               v
             v                          | No              ============================
             |                          v                 | Perform Unjello <11><13> |
             |            +---------------------------+   ============================
             |            | Enable RS Option          |             |
             |            |---------------------------|             |
             |            | see option --RS           |             v
             |            | see ScanFlags_GlobFrameRS |   /---------------------\ No
             |            +-------------+-------------+   | if stabilize needed |-->--+
             |                          |                 \---------------------/     |
             |                          |                           | Yes             |
             |                          |                           v                 |
             |                          |                           |                 |
            <1>                        <1>                         <3>               <2>
             |                          |                           |                 |
             v                          v                           v                 |
     ========================================================================         |
     ||                       Perform Stabilization <10><12>               ||         |
     ========================================================================         |
             |                                                                        |
             v                                                                        v
            End                                                                      End




 ===============================================================================
 =                           Hyper-lapse Process <4>                           =
 ===============================================================================

    /--------------------/
  /  Open Source Video /
 /--------------------/
            |
            v
 +------------------------+
 | Determine FrameStep    |
 | by acceleration factor |
 +------------------------+
            |
            v
  /-------------------\  Yes  +------------------------+
  | If FrameStep <= 1 |------>| No Hyper-lapse,         |
  \-------------------/       | use std. Stabilization |
            | No              +------------------------+
            v
 ..............................................................
 :     Configuration                                          :
 ..............................................................
 :                                                            :
 :  /-------------------\  Yes  +------------------------+    :
 :  | If FrameStep <= 5 |------>| Apply Profile(45) <21> |    :
 :  \-------------------/       +------------------------+    :
 :            | No                        |                   :
 :            v                           |                   :
 :  +------------------------+            v                   :
 :  | Apply Profile(46) <21> |            |                   :
 :  +------------------------+            |                   :
 :            |                           |                   :
 :            +<--------------------------+                   :
 :            |                                               :
 :            v                                               :
 :  /-------------------\  Yes  +---------------------------+ :
 :  | If CMOS Video     |------>| Enable RS Option          | :
 :  \-------------------/       +---------------------------+ :
 :            | No              | see option --RS           | :
 :            |                 | see ScanFlags_GlobFrameRS | :
 :            |                 +---------------------------+ :
 :            |                               |               :
 :            +<----------------------<-------+               :
 :            v                                               :
 :  +----------------------------+                            :
 :  | Set additional options     |                            :
 :  | e.g. Camera Intrinsic <22> |                            :
 :  +----------------------------+                            :
 :                                                            :
 ..............................................................
            |
            v
 +--------------------------------+
 | Analysis Source Video          |
 | See VideoStabilization.h  <23> |
 +--------------------------------+
            |
            v
 +--------------------------------------+
 | Export stabilized Video by           |
 | render every FrameStep-th frame      |
 | See VideoStabilization.h <24>        |
 | Example FrameStep=3 (3 times faster) |
 | Render Frames 0, 3, 6, 9 ...         |
 +--------------------------------------+




 Footer:
 < 1> MercalliCli -s Source.mp4 -t Target.mp4 [additional options] --Stabilize
 < 2> MercalliCli -s Source.mp4 -t Target.mp4 [additional options] --Unjello
 < 3> MercalliCli -s Source.mp4 -t Tmp.mp4 [additional options] --Unjello
      MercalliCli -s Tmp.mp4 -t Target.mp4 [additional options] --Stabilize
 < 4> MercalliCli -s Source.mp4 -t Target.mp4 --FrameStep 3 --StabiProfile HyperlapseX4 [additional options] --Stabilize

 <10> Stabilization Process by VideoStabilization.h
 <11> Unjello Process by VideoUnjello.h
 <12> see VideoStabilization()
 <13> see VideoUnjello()

 <21> see VideoStabilizationApplyStabiProfile()
 <22> see MercalliSetCameraIntrinsic()
 <23> see VideoStabilizationUpdate()
 <24> see VideoStabilizationExport()

  More Flowchart details at:
  - VideoUnjello.h
  - VideoStabilization.h
  - Media.h

 */
#pragma endregion

#pragma region PrintCameraIntrinsic
/**
* @brief Prints the names of all statically available camera intrinsic profiles to the console.
* @return HRESULT S_OK on success, or an error code if fetching names/intrinsics fails.
*/
HRESULT PrintCameraIntrinsic()
{
  HRESULT hr = 0;
  SINT32 Count = MercalliGetStaticCameraIntrinsicNums(); // Get total count of profiles.
  UNICHAR Name[80];
  T_CameraIntrinsic CI;

  printf("Available Camera Intrinsics\n");
  for(SINT32 i=0; i < Count; ++i)
  {
    hr = MercalliGetStaticCameraIntrinsicName(i, Name, sizeof(Name));
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGetStaticCameraIntrinsicName %d failed\n", i);
      return hr;
    }

    hr = MercalliGetStaticCameraIntrinsic(i, &CI);
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGetStaticCameraIntrinsic %d failed\n", i);
      return hr;
    }


    printf("  \"%" PRIws "\"\n", Name);
  }
  return hr;
}
#pragma endregion

#pragma region GetCameraIntrinsicByName
/**
* @brief Retrieves camera intrinsic data (T_CameraIntrinsic) by its profile name.
* @param findName The UTF-8 encoded name of the camera profile to find.
* @param[out] pCI Pointer to T_CameraIntrinsic struct to be filled.
* @return HRESULT S_OK if found and retrieved, E_FAIL if not found, or other error code.
*/
HRESULT GetCameraIntrinsicByName(const char* findName, T_CameraIntrinsic* pCI)
{
  HRESULT hr = 0;
  SINT32 Count = MercalliGetStaticCameraIntrinsicNums();
  UNICHAR CIName[64], FindName[64]; // Buffers for Unicode names.

  // Convert input UTF-8 name to UNICHAR (likely UTF-16 on Windows).
  // Assumes ToUNICHAR is a utility function defined elsewhere (e.g., in stdafx or a common lib).
  hr = ToUNICHAR(findName, FindName, 64); // 64 is character count for UNICHAR buffer
  if(FAILED(hr))
  {
    fprintf(stderr, "ToUNICHAR failed\n");
    return hr;
  }


  for(SINT32 i=0; i < Count; ++i)
  {
    hr = MercalliGetStaticCameraIntrinsicName(i, CIName, sizeof(CIName));
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGetStaticCameraIntrinsicName %d failed\n", i);
      return hr;
    }

    if(wcsicmp(FindName, CIName) == 0)
    {
      hr = MercalliGetStaticCameraIntrinsic(i, pCI);
      if(FAILED(hr))
      {
        fprintf(stderr, "MercalliGetStaticCameraIntrinsic %d failed\n", i);
        return hr;
      }

      return 0;
    }
  }
  return E_FAIL;
}
#pragma endregion

#pragma region VideoStabilization
/* Perform the Video-Stabilization (Smooth Camera Path)
 * \param sourceFileName -- Original Video Filename (e.g. "MyVideo.mp4")
 * \param targetFileName -- Optimized Video Filename (e.g. "Improved.mp4")
 * \param startTime      -- Start-Time of the Stabilization-Operation
 * \param endTime        -- End-Time of the Stabilization-Operation
 * \param frameStep      -- For Hyper-lapse [1,2,3 ... ] 
 * \param profileID      -- StabiProfile (e.g. 42)
 * \param decoderFlags   -- (enum DecoderFlags_Enum)
 */
HRESULT VideoStabilization(
  CVideoStabilizationParam& param,
        bool     verbose,
        bool     usePref,
  FrameType_Enum frameType,
  const char*    sourceFileName,
  const char*    targetFileName,
        float    startTime,
        float    endTime,
        float    frameStep,
        SINT32   profileID,
        UINT32  decoderFlags)
{
  HRESULT hr;

  // set
  param.Verbose   = verbose;
  param.UsePref   = usePref;
  param.FrameType = frameType;

  // open source video
  hr = OpenVideoDecoder(&param.VideoDecoder, sourceFileName, decoderFlags);
  if(FAILED(hr))
  {
    fprintf(stderr, "Could not open video: %s\n", sourceFileName);
    return hr;
  }

  // get source video info
  hr = param.VideoDecoder->GetInfo(&param.SourceMediaInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "Could not get video info: %s\n", sourceFileName);
    return hr;
  }

  // profileID (0 means use default from param or no change)
  if(profileID != 0)
  {
    hr = VideoStabilizationApplyStabiProfile(param, profileID);
    if(FAILED(hr))
    {
      fprintf(stderr, "Could not apply stabi-profile: %d\n", profileID);
      return hr;
    }
  }  

  // FrameStep (ensure it's at least 1)
  param.FrameStep = (SINT32)frameStep;
  param.FrameStep = std::max(1, param.FrameStep);

  // Frame Numbers (convert time in seconds to frame indices)
  param.SourceStartFrameNumber = (SINT32)(startTime * param.SourceMediaInfo.Rate); 
  param.SourceEndFrameNumber   = (SINT32)(endTime   * param.SourceMediaInfo.Rate); 

  param.SourceStartFrameNumber = std::max(0, param.SourceStartFrameNumber);
  param.SourceStartFrameNumber = std::min(param.SourceMediaInfo.FrameCount-1, param.SourceStartFrameNumber);

  if(param.SourceEndFrameNumber <= 0)
    param.SourceEndFrameNumber = param.SourceMediaInfo.FrameCount;
  else
    param.SourceEndFrameNumber = std::max(param.SourceEndFrameNumber, param.SourceStartFrameNumber);

  printf("Video %s\n"
    "  Format:        %dx%d\n"
    "  Rate:          %f f/s\n"
    "  PAR:           %f\n"
    "  Clip Frames:   %d\n"
    "  Field order:   %d\n"
    "  Frame Range:   %d ... %d\n"
    "  Frame Step:    %d\n"
    "  Stabi Profile: %d\n"
    "  Frame type:    %s\n"
   , sourceFileName
   , param.SourceMediaInfo.Width, param.SourceMediaInfo.Height
   , param.SourceMediaInfo.Rate
   , param.SourceMediaInfo.PAR
   , param.SourceMediaInfo.FrameCount
   , param.SourceMediaInfo.FieldOrder
   , param.SourceStartFrameNumber
   , param.SourceEndFrameNumber
   , param.FrameStep
   , param.ProfileID
   , FrameTypeText(param.FrameType)
  );

  if(param.Verbose)
    printf("Call MercalliCreateGmp()\n");

  // create GMP instance. Frame count for GMP is number of frames in the selected range.
  SINT32 gmpFrameCount = (param.SourceEndFrameNumber - param.SourceStartFrameNumber);
  // If FrameStep > 1 for hyperlapse, the GMP might still need to conceptually hold all original frames in range,
  // or it might be (param.SourceEndFrameNumber - param.SourceStartFrameNumber) / param.FrameStep.
  // The current code `param.SourceEndFrameNumber - param.SourceStartFrameNumber` suggests GMP covers all original frames.
  // The flowcharts and MercalliFrameNumberFromMediaFrame support this; FrameStep is handled at analysis/render.
  if(gmpFrameCount <= 0)
  {
    fprintf(stderr, "Invalid frame range resulting in zero or negative GMP frame count.\n");
    return E_INVALIDARG;
  }

  // create instance
  hr = MercalliCreateGmp
  (
    &param.GMP,
    gmpFrameCount,
    (param.SourceMediaInfo.FieldOrder != FLD_Full) ? NewGMPFlags_Fields : 0 // interlaced?
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliCreateGmp failed\n");
    return hr;
  }


  // update all - includes the analysis 
  hr = VideoStabilizationUpdate(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoStabilizationUpdate failed\n");
    return hr;
  }

  if(targetFileName == nullptr) // If no target, it's a simulation (e.g. just analysis).
  {
    // export simulation (might just render without writing, or a dry run).
    // VideoStabilizationExport does create a target frame buffer even if VideoEncoder is null.
    // It seems this path is for testing rendering logic without actual file output.
    hr = VideoStabilizationExport(param);
    if(FAILED(hr))
    {
      fprintf(stderr, "VideoStabilizationExport failed\n");
      return hr;
    }
  }
  else // Actual export to file
  {
    // create target video encoder
    hr = CreateVideoEncoder(&param.VideoEncoder, param.SourceMediaInfo, targetFileName);
    if(FAILED(hr))
    {
      fprintf(stderr, "Could not create video encoder: %s\n", targetFileName);
      return hr;
    }

    // export stabilized video
    hr = VideoStabilizationExport(param);
    if(FAILED(hr))
    {
      fprintf(stderr, "VideoStabilizationExport failed\n");
      return hr;
    }
  }

  //--- Cleanup
  if(param.VideoDecoder != nullptr)
  {
    param.VideoDecoder->Release();
    param.VideoDecoder = nullptr;
  }

  if(param.VideoEncoder != nullptr)
  {
    param.VideoEncoder->Release();
    param.VideoEncoder = nullptr;
  }

  //if(verbose)
  //VideoStabilizationPrintCharts(param);


  return hr;
}
#pragma endregion

#pragma region VideoUnjello
/* Perform the Video-Unjello (Remove CMOS distortions)
 * \param sourceFileName -- Original Video Filename (e.g. "MyVideo.mp4")
 * \param targetFileName -- Optimized Video Filename (e.g. "Improved.mp4")
 * \param startTime      -- Start-Time of the Unjello-Operation
 * \param endTime        -- End-Time of the Unjello-Operation
 */
HRESULT VideoUnjello(
  CVideoUnjelloParam& param,
  bool verbose,
  const char* sourceFileName,
  const char* targetFileName,
  float startTime,
  float endTime)
{
  HRESULT hr;

  // set
  param.Verbose = verbose;

  // open source video
  hr = OpenVideoDecoder(&param.VideoDecoder, sourceFileName, 0); 
  if(FAILED(hr))
  {
    fprintf(stderr, "Could not open video: %s\n", sourceFileName);
    return hr;
  }

  // get source video info
  hr = param.VideoDecoder->GetInfo(&param.SourceMediaInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "Could not get video info: %s\n", sourceFileName);
    return hr;
  }

  // Frame number calculation and clamping - similar to VideoStabilization.
  param.SourceStartFrameNumber = (SINT32)(startTime * param.SourceMediaInfo.Rate);
  param.SourceEndFrameNumber   = (SINT32)(endTime   * param.SourceMediaInfo.Rate);

  // ... clamping logic ...
  param.SourceStartFrameNumber = std::max(0, param.SourceStartFrameNumber);
  param.SourceStartFrameNumber = std::min(param.SourceMediaInfo.FrameCount-1, param.SourceStartFrameNumber);

  if(param.SourceEndFrameNumber <= 0)
    param.SourceEndFrameNumber = param.SourceMediaInfo.FrameCount;
  else
    param.SourceEndFrameNumber = std::max(param.SourceEndFrameNumber, param.SourceStartFrameNumber);

  printf("Video %s\n"
    "  Format:       %dx%d\n"
    "  Rate:         %f f/s\n"
    "  PAR:          %f\n"
    "  Clip Frames:  %d\n"
    "  Field order:  %d\n"
    "  Frame Range:  %d ... %d\n"
   , sourceFileName
   , param.SourceMediaInfo.Width, param.SourceMediaInfo.Height
   , param.SourceMediaInfo.Rate
   , param.SourceMediaInfo.PAR
   , param.SourceMediaInfo.FrameCount
   , param.SourceMediaInfo.FieldOrder
   , param.SourceStartFrameNumber
   , param.SourceEndFrameNumber
  );

  if(param.Verbose)
    printf("Call MercalliCreateUnjello()\n");

  // create instance
  hr = MercalliCreateUnjello
  (
    &param.Unjello,
    param.SourceMediaInfo.Width,
    param.SourceMediaInfo.Height,
    param.SourceMediaInfo.FieldOrder,
    param.SourceMediaInfo.PAR,
    param.SourceMediaInfo.Rate
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliCreateUnjello failed\n");
    return hr;
  }

  // basic setup
  hr = VideoUnjelloSetup(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoUnjelloSetup failed\n");
    return hr;
  }

  // update all - includes the speed estimation 
  hr = VideoUnjelloUpdate(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoUnjelloUpdate failed\n");
    return hr;
  }

  // Print Options to Console
  PrintUnjelloOptions(param);

  // create target video encoder
  hr = CreateVideoEncoder(&param.VideoEncoder, param.SourceMediaInfo, targetFileName); 
  if(FAILED(hr))
  {
    fprintf(stderr, "Could not create video encoder: %s\n", targetFileName);
    return hr;
  }

  // export unjello-video
  hr = VideoUnjelloExport(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoUnjelloExport failed\n");
    return hr;
  }

  //--- Cleanup
  if(param.VideoDecoder != nullptr)
  {
    param.VideoDecoder->Release();
    param.VideoDecoder = nullptr;
  }

  if(param.VideoEncoder != nullptr)
  {
    param.VideoEncoder->Release();
    param.VideoEncoder = nullptr;
  }

  return hr;
}
#pragma endregion

#pragma region TestDecEnc
/* Test Video Decode/Encode
 */
HRESULT TestDecEnc(
  const char* sourceFileName,
  const char* targetFileName)
{
  HRESULT hr = 0;
  IVideoDecoder* Decoder = nullptr;
  IVideoEncoder* Encoder = nullptr;
  CMediaInfo MediaInfo;

  hr = OpenVideoDecoder(&Decoder, sourceFileName, 0);
  if(SUCCEEDED(hr))
    hr = Decoder->GetInfo(&MediaInfo);
  if(SUCCEEDED(hr))
    hr = CreateVideoEncoder(&Encoder, MediaInfo, targetFileName);
  if(SUCCEEDED(hr))
  {
    for(SINT32 i=0; i < MediaInfo.FrameCount; ++i)
    {
      IVideoFrame* Frame = nullptr;
      hr = Decoder->ReadFrame(&Frame, i, MediaInfo.Width, MediaInfo.Height, FMT_BGR8);
      if(SUCCEEDED(hr))
        hr = Encoder->WriteFrame(Frame);

      if(Frame != nullptr)
        Frame->Release();

      if(FAILED(hr))
        break;
      
      printf("\rExport: %d%%  ", (int)(100*i / MediaInfo.FrameCount) );
    }
  }

  if(Decoder != nullptr)
    Decoder->Release();
  if(Encoder != nullptr)
    Encoder->Release();
  
  printf("\rExport finished   \n");
  return hr;
}
#pragma endregion

#pragma region GetFloatArg
static HRESULT GetFloatArg(int argc, char* argv[], int index, float* pValue)
{
  if(index >= argc || strlen(argv[index]) == 0)
  {
    printf("Err: Missing value after option: %s\n", argv[index-1]);
    return E_FAIL;
  }
  *pValue = (float)atof(argv[index]);
  return 0;
}
#pragma endregion

#pragma region GetBoolArg
static HRESULT GetBoolArg(int argc, char* argv[], int index, bool* pValue)
{
  if(index >= argc || strlen(argv[index]) == 0)
  {
    printf("Err: Missing value after option: %s\n", argv[index-1]);
    return E_FAIL;
  }
  if(stricmp(argv[index], "on") == 0 || stricmp(argv[index], "true") == 0)
    *pValue = true;
  else if(stricmp(argv[index], "off") == 0 || stricmp(argv[index], "false") == 0)
    *pValue = false;
  else
  {
    printf("Err: Unsupported BOOL option %s for argument %s: Should ON|OFF or TRUE|FALSE\n"
      , argv[index], argv[index-1]);
    return E_FAIL;
  }
  return 0;
}
#pragma endregion

#pragma region GetTextArg
static HRESULT GetTextArg(int argc, char* argv[], int index, const char** ppValue)
{
  if(index >= argc || strlen(argv[index]) == 0)
  {
    printf("Err: Missing value after option: %s\n", argv[index-1]);
    return E_FAIL;
  }
  *ppValue = argv[index];
  return 0;
}
#pragma endregion

#pragma region Help
static const char TitleText[]=
"+++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
"+++ MercalliCli %d.%d.%d.%d (c)2006-2025 by proDAD +++\n"
"+++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
;

static const char HelpText[]=
"Usage:\n"
"MercalliCli -s <path> -t <path> [options] --Stabilize\n"
"or\n"
"MercalliCli -s <path> -t <path> [options] --Unjello\n"
"\n"
"Options:\n"
"-h or --Help                  :: display this help\n"
"--verbose [ON|OFF]\n"
"--license <liz>               :: Add a license\n"
"-s or --Source <path>         :: (Source video file-name or directory-name if not windows)\n"
"-t or --Target <path>         :: (Target video file-name or directory-name if not windows)\n"
"--in  <start-time in seconds> :: e.g.: 1.5\n"
"--out <end-time in seconds>   :: e.g.: 6.5\n"
"--List-CameraIntrinsic        :: List available camera names\n"
"\n"
"Commandos:\n"
"--Stabilize                   :: perform video stabilization\n"
"                                 Smooth camera path\n"
"--Unjello                     :: perform CMOS unjello\n"
"                                 Sub-Frame CMOS-distortions remover\n"
"\n"
"Options for --Stabilize:\n"
"--Source-Camera <name>        :: set source camera.\n"
"                                 Name is one of \"--List-CameraIntrinsic\"\n"
"--Target-Camera <name>        :: set target camera.\n"
"                                 Name is one of \"--List-CameraIntrinsic\"\n"
"--Camera <name>               :: set source and target camera.\n"
"                                 Name is one of \"--List-CameraIntrinsic\"\n"

"--RS [ON|OFF]                 :: full-frame rolling-shutter compensation\n"
"--TranslationSmoothing <0.0 ... 1.0>\n"
"--ZRotationSmoothing <0.0 ... 1.0>\n"
"--ViewScale <0.0 ... 1.0>\n"
"--BorderMode <mode>           :: [Suppress|DynCrop|Forensic|Raw]\n"
"--FrameStep <frame-count>     :: Interleave Frames for Stabilization [1,2, ...]\n"
"--Profile <mode>              :: [Turbo|AI|AI3D]\n"
"\n"
"Options for --Unjello:\n"
"--ShutterSpeed <1.0 ... >     :: Manual set sensor shutter speed\n"
"--AutoShutterSpeed [ON|OFF]   :: Automatic sensor shutter speed detection\n"
"--EstimateShutterSpeed [ON|OFF]:: Estimate the sensor shutter speed as a first Pass\n"
"--UJMth <mode>                :: Unjello Method [P-OF|P-FP]\n"
"                                 P-OF requires GPU\n"
"\n"
;

void PrintHelp()
{
  printf
  (
    TitleText
   , (int)((MercalliVersion() >> 48) & 0xffff) // Main Version
   , (int)((MercalliVersion() >> 32) & 0xffff) // Sub Version
   , (int)((MercalliVersion() >> 16) & 0xffff) // Tek/Revision
   , (int)((MercalliVersion() >>  0) & 0xffff) // Patch
  );
  printf(HelpText);
}

#pragma endregion

#pragma region main
int main(int argc, char* argv[])
{
  #pragma region proDAD internals
  #if defined(MEDULLA_LINK_DYNLIB) || defined(MEDULLA_LINK_STATICLIB)
  CBootstrap Bootstrap;
  #endif
  #pragma endregion

  #pragma region Variables
  HRESULT hr = 0;
  CVideoStabilizationParam StabiParam;
  CVideoUnjelloParam       UnjelloParam;
  T_CameraIntrinsic CameraIntrinsic;
  const char* SourceFileName = nullptr;
  const char* TargetFileName = nullptr;
  float StartTime = 0;
  float EndTime = 0;
  float FrameStep = 0;
  bool Verbose = false;
  bool UsePref = false;
  SINT32 StabiProfileID = 0;
  FrameType_Enum FrameType = FrameType_Standard;
  UINT32 DecoderFlags = 0; // (enum DecoderFlags_Enum)

  #pragma endregion

  #pragma region Platform Characteristics
  #ifdef OS_MACOS10
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  #endif  
  #pragma endregion
  
  #pragma region proDAD internals (Bootstrap)
  #if defined(MEDULLA_LINK_DYNLIB) || defined(MEDULLA_LINK_STATICLIB)
  using namespace proDAD::Medulla;

  if(FAILED(Bootstrap.Startup()))
  {
    printf("Startup failed. Please check WorkDirectory.\n");
    return 20;
  }
  #if defined(MEDULLA_LINK_STATICLIB)
    #if defined(NDEBUG)
    SetGlobalObserverLevel(OBSPLV_Warn | OBSPLV_Error);
    #else
    SetGlobalObserverLevel(OBSPLV_Verbose | OBSPLV_Warn | OBSPLV_Error);
    #endif
  #endif
  
  #endif
  #pragma endregion

  #pragma region Mercalli Version Check
  if(!MercalliUsable())
  {
    printf("Error: Unsupported Mercalli Version\n");
    return 20;
  }
  #pragma endregion
  
  #pragma region Read Args
  // check arguments
  if(argc < 2)
  {
    PrintHelp();
    return 1;
  }

  /* Workaround for gcc with pragma inner if else
   * gcc does not support the inner if else
   * so we have to use a workaround with else if
   * and a comment in the else if
   * this is only for the help text
   * the rest of the code is ok
  */

  bool ArgUsed = false;

  for(int ac = 1; ac < argc; ++ac)
  {
    #pragma region Help
    if(  stricmp(argv[ac], "-h") == 0 
      || stricmp(argv[ac], "-help") == 0 
      || stricmp(argv[ac], "--help") == 0
      || stricmp(argv[ac], "help") == 0
    )
    {
      PrintHelp();
      return 1;
    }
    #pragma endregion
    #pragma region --license
    /*else*/ if(stricmp(argv[ac], "--license") == 0)
    {
      const char* Liz{};
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &Liz) ))
        return 20;
      if(FAILED( MercalliAddLicense(Liz) ))
      {
        printf("Error: Could not add license: wrong or invalid\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region -s  --source
    /*else*/ if(stricmp(argv[ac], "-s") == 0 || stricmp(argv[ac], "--source") == 0)
    {
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &SourceFileName) ))
        return 20;
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region -t  --target
    /*else*/ if(stricmp(argv[ac], "-t") == 0 || stricmp(argv[ac], "--target") == 0)
    {
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &TargetFileName) ))
        return 20;
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --in
    /*else*/ if(stricmp(argv[ac], "--in") == 0)
    {
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &StartTime) ))
        return 20;
      ArgUsed = true;

      if(StartTime < 0)
      {
        printf("Error: Wrong StartTime: %s\n", argv[ac]);
        return 20;
      }
    }
    #pragma endregion
    #pragma region --out
    /*else*/ if(stricmp(argv[ac], "--out") == 0)
    {
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &EndTime) ))
        return 20;
      ArgUsed = true;

      if(EndTime < 0)
      {
        printf("Error: Wrong EndTime: %s\n", argv[ac]);
        return 20;
      }
    }
    #pragma endregion
    #pragma region --framestep
    /*else*/ if(stricmp(argv[ac], "--framestep") == 0)
    {
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &FrameStep) ))
        return 20;
      ArgUsed = true;

      if(FrameStep < 1)
      {
        printf("Error: Wrong FrameStep: %s\n", argv[ac]);
        return 20;
      }

      if(FrameStep != (int)FrameStep)
      {
        printf("Warn: FrameStep will be used as Integer\n");
      }
    }
    #pragma endregion
    #pragma region --frametype --ft --cl
    /*else*/ if(stricmp(argv[ac], "--frametype") == 0 || stricmp(argv[ac], "--ft") == 0)
    {
      const char* Arg{};
      ++ac;
      if(FAILED(GetTextArg(argc, argv, ac, &Arg)))
        return 20;
      ArgUsed = true;

      if(strcmp(Arg, "standard") == 0)
        FrameType = FrameType_Standard;
      else if(strcmp(Arg, "cpu") == 0)
        FrameType = FrameType_CPU;
      else if(strcmp(Arg, "cl") == 0)
        FrameType = FrameType_CL;

      else
      {
        printf("Error: Unsupported Frametype: %s\n", argv[ac]);
      }
    }
    else if(stricmp(argv[ac], "--cl") == 0)
    {
      FrameType = FrameType_CL;
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region  --SwapFields
    /*else*/ if(stricmp(argv[ac], "--SwapFields") == 0)
    {
      DecoderFlags ^= DecF_SwapFields;
      ArgUsed = true;
    }
    #pragma endregion

    #pragma region --verbose
    /*else*/ if(stricmp(argv[ac], "--verbose") == 0)
    {
      ++ac;
      if(FAILED( GetBoolArg(argc, argv, ac, &Verbose) ))
        return 20;
      ArgUsed = true;

#if defined(MEDULLA_LINK_STATICLIB)
      if(Verbose)
        SetGlobalObserverLevel(OBSPLV_Verbose | OBSPLV_Warn | OBSPLV_Error);
#endif
    }
    #pragma endregion
    #pragma region --pref
    /*else*/ if(stricmp(argv[ac], "--pref") == 0)
    {
      ++ac;
      if(FAILED(GetBoolArg(argc, argv, ac, &UsePref)))
        return 20;
      ArgUsed = true;
    }
    #pragma endregion


    #pragma region --rs
    /*else*/ if(stricmp(argv[ac], "--rs") == 0)
    {
      bool Value;
      ++ac;
      if(FAILED( GetBoolArg(argc, argv, ac, &Value) ))
        return 20;
      StabiParam.EnableGlobFrameRS(Value);
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --List-CameraIntrinsic
    /*else*/ if(stricmp(argv[ac], "--List-CameraIntrinsic") == 0)
    {
      PrintCameraIntrinsic();
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --Source-Camera
    /*else*/ if(stricmp(argv[ac], "--Source-Camera") == 0)
    {
      ++ac;
      if(ac >= argc || strlen(argv[ac]) == 0)
      {
        printf("Error: Missing name after option: %s\n", argv[ac-1]);
        return 20;
      }
      ArgUsed = true;

      hr = GetCameraIntrinsicByName(argv[ac], &CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("Could not find %s\n", argv[ac]);
        return 20;
      }

      hr = StabiParam.SetSourceCameraIntrinsic(CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("SetSourceCameraIntrinsic failed\n");
        return 20;
      }
    }
    #pragma endregion
    #pragma region --Target-Camera
    /*else*/ if(stricmp(argv[ac], "--Target-Camera") == 0)
    {
      ++ac;
      if(ac >= argc || strlen(argv[ac]) == 0)
      {
        printf("Error: Missing name after option: %s\n", argv[ac-1]);
        return 20;
      }
      ArgUsed = true;

      hr = GetCameraIntrinsicByName(argv[ac], &CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("Could not find %s\n", argv[ac]);
        return 20;
      }

      hr = StabiParam.SetTargetCameraIntrinsic(CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("SetTargetCameraIntrinsic failed\n");
        return 20;
      }
    }
    #pragma endregion
    #pragma region --Camera
    /*else*/ if(stricmp(argv[ac], "--Camera") == 0)
    {
      ++ac;
      if(ac >= argc || strlen(argv[ac]) == 0)
      {
        printf("Error: Missing name after option: %s\n", argv[ac-1]);
        return 20;
      }
      ArgUsed = true;

      hr = GetCameraIntrinsicByName(argv[ac], &CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("Could not find %s\n", argv[ac]);
        return 20;
      }

      hr = StabiParam.SetSourceCameraIntrinsic(CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("SetSourceCameraIntrinsic failed\n");
        return 20;
      }
      hr = StabiParam.SetTargetCameraIntrinsic(CameraIntrinsic);
      if(FAILED(hr))
      {
        printf("SetTargetCameraIntrinsic failed\n");
        return 20;
      }
    }
    #pragma endregion
    #pragma region --TranslationSmoothing
    /*else*/ if(stricmp(argv[ac], "--TranslationSmoothing") == 0)
    {
      float Value;
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &Value) ))
        return 20;
      hr = StabiParam.SetTranslationSmoothing(Value);
      if(FAILED(hr))
      {
        printf("Error: SetTranslationSmoothing failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --ZRotationSmoothing
    /*else*/ if(stricmp(argv[ac], "--ZRotationSmoothing") == 0)
    {
      float Value;
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &Value) ))
        return 20;
      hr = StabiParam.SetZRotationSmoothing(Value);
      if(FAILED(hr))
      {
        printf("Error: SetZRotationSmoothing failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --ViewScale
    /*else*/ if(stricmp(argv[ac], "--ViewScale") == 0)
    {
      float Value;
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &Value) ))
        return 20;
      hr = StabiParam.SetViewScale(Value);
      if(FAILED(hr))
      {
        printf("Error: SetViewScale failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --BorderMode
    /*else*/ if(stricmp(argv[ac], "--BorderMode") == 0)
    {
      BorderParam_Enum B;
      const char* Value{};
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &Value) ))
        return 20;
      if(stricmp(Value, "Suppress")==0)
        B = BorderParam_Suppress;
      else if(stricmp(Value, "DynCrop")==0)
        B = BorderParam_DynCrop;
      else if(stricmp(Value, "Raw") == 0)
        B = BorderParam_Raw;
      else if(stricmp(Value, "Forensic") == 0)
        B = BorderParam_Forensic;
      else
      {
        printf("Error: Unsupported BorderMode: %s\n", Value);
        return 20;
      }


      hr = StabiParam.SetBorderMode(B);
      if(FAILED(hr))
      {
        printf("Error: SetBorderMode failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --Profile
    /*else*/ if(stricmp(argv[ac], "--Profile") == 0)
    {
      const char* Value{};
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &Value) ))
        return 20;
      if(stricmp(Value, "Universal")==0 || stricmp(Value, "Turbo") == 0)
        StabiProfileID = 42;
      else if(stricmp(Value, "AI")==0)
        StabiProfileID = 44;
      else if(stricmp(Value, "AI3D")==0)
        StabiProfileID = 45;
      else
      {
        printf("Error: Unsupported StabiProfile: %s\n", Value);
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion

    #pragma region --ShutterSpeed
    /*else*/ if(stricmp(argv[ac], "--ShutterSpeed") == 0)
    {
      float Value;
      ++ac;
      if(FAILED( GetFloatArg(argc, argv, ac, &Value) ))
        return 20;

      if(Value < 1)
      {
        printf("Error: Wrong ShutterSpeed: [1.0 ... ]\n");
        return 20;
      }

      hr = UnjelloParam.SetShutterSpeed(Value);
      if(FAILED(hr))
      {
        printf("Error: SetShutterSpeed failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --AutoShutterSpeed
    /*else*/ if(stricmp(argv[ac], "--AutoShutterSpeed") == 0)
    {
      bool Value;
      ++ac;
      if(FAILED( GetBoolArg(argc, argv, ac, &Value) ))
        return 20;

      hr = UnjelloParam.EnableAutoShutterSpeed(Value);
      if(FAILED(hr))
      {
        printf("Error: EnableAutoShutterSpeed failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --EstimateShutterSpeed
    /*else*/ if(stricmp(argv[ac], "--EstimateShutterSpeed") == 0)
    {
      bool Value;
      ++ac;
      if(FAILED( GetBoolArg(argc, argv, ac, &Value) ))
        return 20;

      hr = UnjelloParam.EnableEstimateShutterSpeed(Value);
      if(FAILED(hr))
      {
        printf("Error: EnableEstimateShutterSpeed failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --UJMth
    /*else*/ if(stricmp(argv[ac], "--UJMth") == 0)
    {
      UnjelloMethodType_Enum M;
      const char* Value{};
      ++ac;
      if(FAILED( GetTextArg(argc, argv, ac, &Value) ))
        return 20;
      if(stricmp(Value, "PTek_OF")==0      || stricmp(Value, "P-OF")==0)
        M = UJMthTyp_PTek_OF;
      else if(stricmp(Value, "PTek_FP")==0 || stricmp(Value, "P-FP")==0)
        M = UJMthTyp_PTek_FP;
      else
      {
        printf("Error: Unsupported Unjello-Method: %s\n", Value);
        return 20;
      }


      hr = UnjelloParam.SetMethod(M);
      if(FAILED(hr))
      {
        printf("Error: SetMethod failed\n");
        return 20;
      }
      ArgUsed = true;
    }
    #pragma endregion


    #pragma region --stabilize
    /*else*/ if(stricmp(argv[ac], "--stabilize") == 0)
    {
      if(!SourceFileName)
      {
        printf("Missing Source filename\n");
        return 20;
      }

      hr = VideoStabilization
      (
        StabiParam,
        Verbose,
        UsePref,
        FrameType,
        SourceFileName,
        TargetFileName,
        StartTime,
        EndTime,
        FrameStep,
        StabiProfileID,
        DecoderFlags
      );
      ArgUsed = true;
    }
    #pragma endregion
    #pragma region --unjello
    /*else*/ if(stricmp(argv[ac], "--unjello") == 0)
    {
      if(!SourceFileName || !TargetFileName)
      {
        printf("Error: Missing Source/Target filename\n");
        return 20;
      }

      hr = VideoUnjello(UnjelloParam, Verbose, SourceFileName, TargetFileName, StartTime, EndTime);
      ArgUsed = true;
    }
    #pragma endregion

    #ifdef _EXTEST_
    #include "ExTest.h"
    #endif

    if(!ArgUsed)
    {
      printf("Warn: Unknown option: %s (try -h to display help)\n", argv[ac]);
      //return 20;
    }

    if(FAILED(hr))
      break;
  }
  //END Read Args
  #pragma endregion

  #pragma region Platform Characteristics
  #ifdef OS_MACOS10
    [pool drain];
  #endif
  #pragma endregion

  #pragma region Error Check and Edit
  if(FAILED(hr))
  {
    printf("+++ failed +++\n");
    return 20;
  }

  return 0;
  #pragma endregion
}
//END main
#pragma endregion

