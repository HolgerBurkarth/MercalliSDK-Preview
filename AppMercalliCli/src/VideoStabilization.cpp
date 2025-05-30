#pragma region Prolog
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>VideoStabilization.cpp<< 30 Okt 2020  10:37:29 - (c) proDAD
*******************************************************************/
#pragma endregion
#pragma region Includes
#include "stdafx.h"

#pragma endregion

#pragma region CVideoStabilizationParam Construction
CVideoStabilizationParam::~CVideoStabilizationParam()
{
  if(GMP != nullptr)
  {
    if(Verbose)
      printf("Call MercalliDeleteGmp()\n");
    MercalliDeleteGmp(GMP);
  }

  if(DataStream != nullptr)
  {
    if(Verbose)
      printf("Call MercalliFreeDataStream()\n");
    MercalliFreeDataStream(DataStream);
  }

  if(VideoDecoder != nullptr)
    VideoDecoder->Release();
  if(VideoEncoder != nullptr)
    VideoEncoder->Release();
}

CVideoStabilizationParam::CVideoStabilizationParam()
{
  // --- Set Defaults ---
  MercalliApplyProfile(ProfileID, &Settings);

  InitCamera(&SourceCameraIntrinsic);
  InitCamera(&TargetCameraIntrinsic);
}

#pragma endregion

#pragma region PrintMercalliDataStream
/* Prints detailed information from the Mercalli DataStream to the console
 */
void PrintMercalliDataStream(CVideoStabilizationParam& param)
{
  HRESULT hr;
  T_MercalliStreamMediaInfo*       MediaInfo;
  T_MercalliStreamCameraIntrinsic* CameraIntrinsic;
  T_MercalliStreamFrameMatrixKey*  FrameMatrixKey;
  T_MercalliStreamSettingsB*       SettingsB;

  hr = MercalliGetStreamMediaInfo(param.DataStream, &MediaInfo);
  if(SUCCEEDED(hr))
  {
    printf(
      "MediaInfo:\n"
      "   Format:     %dx%d\n"
      "   PAR:        %f\n"
      "   Rate:       %f f/s\n"
      "   FieldOrder: %d\n"
      , MediaInfo->MediaWidth
      , MediaInfo->MediaHeight
      , MediaInfo->PAR
      , MediaInfo->Rate
      , MediaInfo->FieldOrder
    );
  }

  hr = MercalliGetStreamCameraIntrinsic(param.DataStream, &CameraIntrinsic);
  if(SUCCEEDED(hr))
  {
    const T_CameraIntrinsic* Src = &CameraIntrinsic->SourceCameraIntrinsic;
    const T_CameraIntrinsic* Tar = &CameraIntrinsic->TargetCameraIntrinsic;
    printf(
      "CameraIntrinsic:\n"
      "   Source: %dx%d  f: %f; %f\n"
      "   Target: %dx%d  f: %f; %f\n"
      , Src->Width, Src->Height, Src->Fx, Src->Fy
      , Tar->Width, Tar->Height, Tar->Fx, Tar->Fy
    );
  }

  hr = MercalliGetStreamSmoothFrameMatrix(param.DataStream, &FrameMatrixKey);
  if(SUCCEEDED(hr))
  {
    printf(
      "FrameMatrixKey:\n"
      "   Count: %d\n"
      , (int)FrameMatrixKey->Count
    );
  }

  hr = MercalliGetStreamSettingsB(param.DataStream, &SettingsB);
  if(SUCCEEDED(hr))
  {
    const T_SettingsB* S = &SettingsB->CurrentSettings;
    printf(
      "SettingsB:\n"
      "   ProfileID:      %d\n"

      "   GlobFrameRS:    %s (RollingShutter)\n"
      "   DynamicCamera:  %s\n"
      "   XRotationAxis:  %s\n"
      "   YRotationAxis:  %s\n"

      "   TransFac:       %d %%\n"
      "   ZRotationFac:   %d %%\n"
      "   XRotationFac:   %d %%\n"
      "   YRotationFac:   %d %%\n"
      "   ZoomFac:        %d %%\n"
      "   DynScaleFac:    %d %%\n"
      "   ViewScale:      %d %% (Suppress Factor)\n"

      "   BorderParam:    %d\n"
      , SettingsB->CurrentProfileID

      , (S->ScanFlags & ScanFlags_GlobFrameRS)   ? "On" : "Off"
      , (S->ViewFlags & ViewFlags_DynamicCamera) ? "On" : "Off"
      , (S->ViewFlags & ViewFlags_XRotationAxis) ? "On" : "Off"
      , (S->ViewFlags & ViewFlags_YRotationAxis) ? "On" : "Off"

      , S->TransFac * 100 / 65536
      , S->ZRotationFac * 100 / 65536
      , S->XRotationFac * 100 / 65536
      , S->YRotationFac * 100 / 65536
      , S->ZoomFac * 100 / 65536
      , S->DynScaleFac * 100 / 65536
      , S->ViewScale * 100 / 65536

      , S->BorderParam
    );
  }

}
#pragma endregion

#pragma region BuildAnalysisStatusLine
/**
 * @brief Builds a text-based status line for visualizing analysis progress (translation).
 * @param line Output buffer for the status line (should be at least 70 chars).
 * @param t Array of 3 floats for translation (X, Y, Z), only T[0] and T[1] are used.
 * @param r Array of 3 floats for rotation (not used in this function).
 */
static void BuildAnalysisStatusLine(char (&line)[70], FLOAT32 t[3], FLOAT32 r[3])
{
  (void)r; // Unused parameter, suppress warning
  const int LineSize = static_cast<int>(sizeof(line));
  const int Mid = LineSize /2 - 1;
  memset(line, '_', LineSize -1); // Fills buffer with '_'
  line[LineSize -1] = 0;

  // Scaling factor 1.5f is arbitrary, for visualization.
  int X = Mid + (int)(t[0] * 1.5f);
  int Y = Mid + (int)(t[1] * 1.5f);

  if(X < 0)
    X = 0;
  if(Y < 0)
    Y = 0;
  if(X >= LineSize -1)
    X = LineSize -2;
  if(Y >= LineSize -1)
    Y = LineSize -2;

  line[Mid] = '|';
  line[X] = 'X';
  line[Y] = 'Y';
}
#pragma endregion

#pragma region VideoStabilizationApplyStabiProfile
/* Applies a stabilization profile
 */
HRESULT VideoStabilizationApplyStabiProfile(CVideoStabilizationParam& param, SINT32 id)
{
  HRESULT hr = 0;
  if(param.ProfileID != id)
  {
    SINT32 ResidentScanFlags = (param.Settings.ScanFlags & ScanFlags_GlobFrameRS);

    param.ProfileID = id;
    if(!MercalliApplyProfile(param.ProfileID, &param.Settings))
      hr = E_FAIL;
    else
    {
      // If the new profile doesn't support variation, disable it in params
      if(!(param.Settings.ScanFlags & ScanFlags_TryVariation))
        param.SupportVariation = false;

      param.Settings.ScanFlags |= ResidentScanFlags;
    }
  }
  return hr;
}
#pragma endregion

#pragma region VideoStabilizationSetCam
/**
* @brief Sets the source and target camera intrinsic parameters in the GMP.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationSetCam(CVideoStabilizationParam& param)
{
  HRESULT hr;

  if(param.Verbose)
    printf("Call MercalliSetCameraIntrinsic\n");

  // Source Cam
  hr = MercalliSetCameraIntrinsic(param.GMP, CamIticLev_Source, &param.SourceCameraIntrinsic);

  // Target Cam
  if(SUCCEEDED(hr))
    hr = MercalliSetCameraIntrinsic(param.GMP, CamIticLev_Target, &param.TargetCameraIntrinsic);

  if(FAILED(hr))
    fprintf(stderr, "MercalliSetCameraIntrinsic failed\n");
  return hr;
}
#pragma endregion

#pragma region VideoStabilizationAnalysis
/**
* @brief Performs video analysis frame by frame to generate motion data within the GMP.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationAnalysis(CVideoStabilizationParam& param)
{
  assert(param.FrameStep > 0 && param.SourceStartFrameNumber < param.SourceEndFrameNumber);
  HRESULT hr;
  T_GmpScanImageParam Scan{};
  SINT64 LastPrintedFrameNumber = 0;
  SINT64 AnalyzedFrameCount = 0;
  SINT32 AnalysisFrameCount = param.SourceEndFrameNumber - param.SourceStartFrameNumber;
  const format_type Format = FMT_Gray8;
  #if USE_GetPerformanceTime
  FLOAT64 Pref = 0;
  #endif

  if(param.Verbose || param.UsePref)
    printf("Start Video Analysis\n");

  Scan.Settings  = param.Settings;
  Scan.Rate      = param.SourceMediaInfo.Rate;
  Scan.Field     = param.SourceMediaInfo.FieldOrder;

  /*
  * For interlaced video, each frame has two fields, potentially doubling the items to
  * analyze for progress calculation.
  */
  if(Scan.Field != FLD_Full)
    AnalysisFrameCount *= 2;

  //--- set Cams
  hr = VideoStabilizationSetCam(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoStabilizationSetCam failed\n");
    return hr;
  }

  #if USE_GetPerformanceTime
  Pref = GetPerformanceTime();
  #endif

  // for each frame
  for(SINT32 VideoReadFrameNumber = param.SourceStartFrameNumber; VideoReadFrameNumber < param.SourceEndFrameNumber; ++VideoReadFrameNumber)
  {
    char LineText[70];
    IVideoFrame* Frame = nullptr;
    CVideoFrameInfo Info;

    // read Frame
    hr = param.VideoDecoder->ReadFrame
    (
      &Frame,
      VideoReadFrameNumber,
      param.SourceMediaInfo.Width,
      param.SourceMediaInfo.Height,
      Format
    );
    if(FAILED(hr))
    {
      fprintf(stderr, "ReadFrame %d failed\n", VideoReadFrameNumber);
      return hr;
    }
    assert(Frame != nullptr);

    // get Frame info
    hr = Frame->GetInfo(&Info);
    if(FAILED(hr))
    {
      fprintf(stderr, "Frame::GetInfo %d failed\n", VideoReadFrameNumber);
      Frame->Release(); // Free Source Image
      return hr;
    }

    // set Param
    Scan.Image = Info;

    //if(param.Verbose)
    //  printf("  Call MercalliGmpScanImage %d\n", VideoReadFrameNumber);

    /* get Scan-FrameNumber from VideoReadFrameNumber
     * See also \ref VideoStabilizationRenderFrame
     */
    SINT64 ScanFrameNumber = MercalliFrameNumberFromMediaFrame(param, VideoReadFrameNumber);

    // Scan Image
    hr = MercalliGmpScanImage(param.GMP, ScanFrameNumber, &Scan);
    Frame->Release(); // Free Source Image
    Frame = nullptr;

    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGmpScanImage %" PRId64 " failed\n", ScanFrameNumber);
      return hr;
    }


    /* -- Print Analysis Progress -- */
    if(!param.UsePref)
    {
      // what is already finished?
      hr = MercalliGetCurrentAnalysisFrameCount(param.GMP, &AnalyzedFrameCount);
      if(FAILED(hr))
      {
        fprintf(stderr, "MercalliGetCurrentAnalysisFrameCount failed\n");
        return hr;
      }

      // Print info for each frame
      for(; LastPrintedFrameNumber < AnalyzedFrameCount; ++LastPrintedFrameNumber)
      {
        FLOAT32 T[3], R[3];
        hr = MercalliGetAnalysisFrameTranslation(param.GMP, LastPrintedFrameNumber, T);
        if(FAILED(hr))
        {
          fprintf(stderr, "MercalliGetAnalysisFrameTranslation failed\n");
          return hr;
        }

        hr = MercalliGetAnalysisFrameRotation(param.GMP, LastPrintedFrameNumber, R);
        if(FAILED(hr))
        {
          fprintf(stderr, "MercalliGetAnalysisFrameRotation failed\n");
          return hr;
        }

        BuildAnalysisStatusLine(LineText, T, R);

        if(param.Verbose)
          printf("%4" PRId64 " %s\n", LastPrintedFrameNumber, LineText);
        else
          printf("\r%3d%%%s", (int)(100*(LastPrintedFrameNumber-param.SourceStartFrameNumber)/AnalysisFrameCount), LineText);

        fflush(stdout);
      }
    }
  }

  #if USE_GetPerformanceTime
  Pref = GetPerformanceTime() - Pref;

  if(param.UsePref)
    printf("+++Pref VideoStabilizationAnalysis %0.1ff/s  %d Frames\n\n", AnalysisFrameCount / Pref, AnalysisFrameCount);
  else
  #endif
  {
    if(param.Verbose)
      printf("End Video Analysis\n");
    else
      printf("\nVideo Analysis finished\n");
  }
  return 0;
}
#pragma endregion

#pragma region UpdateMercalliDataStreamSettingsB
/**
* @brief Updates the settings (T_SettingsB) and profile ID within the Mercalli data stream.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT UpdateMercalliDataStreamSettingsB(CVideoStabilizationParam& param)
{
  HRESULT hr;

  //--- Remember Settings
  T_MercalliStreamSettingsB* SettingsB;
  hr = MercalliGetStreamSettingsB(param.DataStream, &SettingsB);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetStreamSettingsB failed\n");
    return hr;
  }
  SettingsB->CurrentProfileID = param.ProfileID;
  SettingsB->CurrentSettings  = param.Settings;

  SettingsB->NewProfileID = param.ProfileID;
  SettingsB->NewSettings  = param.Settings;

  return hr;
}
#pragma endregion

#pragma region UpdateMercalliDataStream
/**
* @brief Recreates the Mercalli data stream, serializing current GMP data
* (media info, smoothed matrices, camera intrinsics, settings) into it.
* This is similar to a serialization process for the stabilization state.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT UpdateMercalliDataStream(CVideoStabilizationParam& param)
{
  HRESULT hr;

  // Required Tags
  static const T_MercalliStreamTag Tags[]
  {
    { MercalliStreamKey_MediaInfo         },
    { MercalliStreamKey_SmoothFrameMatrix },
    { MercalliStreamKey_CameraIntrinsic   },
    { MercalliStreamKey_SettingsB         },
  };

  // Free a exists Stream first
  if(param.DataStream != nullptr)
  {
    if(param.Verbose)
      printf("Call MercalliFreeDataStream()\n");
    MercalliFreeDataStream(param.DataStream);
    param.DataStream = nullptr;
  }

  if(param.Verbose)
    printf("Call MercalliCreateDataStream()\n");

  // Create a new valid Data-Stream.
  param.DataStream = MercalliCreateDataStream
  (
    param.GMP,
    Tags,
    sizeof(Tags) / sizeof(Tags[0]),
    &hr
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliCreateDataStream failed\n");
    return hr;
  }

  /* Explicitly update/store settings in the newly created DataStream,
  * as MercalliCreateDataStream might populate it from GMP which might
  * not reflect the very latest `param.Settings`
  */
  hr = UpdateMercalliDataStreamSettingsB(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "UpdateMercalliDataStreamSettingsB failed\n");
    return hr;
  }

  return hr;
}
#pragma endregion

#pragma region VideoStabilizationUpdateRequirement
/**
* @brief Determines if video re-analysis or re-application of settings (smoothing)
* is required based on changes to camera intrinsics or stabilization settings.
* @param param The video stabilization parameters.
* @param[out] pAnalysisRequired True if re-analysis is needed.
* @param[out] pFinishRequired True if re-application of settings (smoothing) is needed.
* @return HRESULT indicating success or failure (typically S_OK unless stream access fails).
*/
HRESULT VideoStabilizationUpdateRequirement(
  CVideoStabilizationParam& param,
  bool* pAnalysisRequired,
  bool* pFinishRequired)
{
  assert(pAnalysisRequired != nullptr && pFinishRequired != nullptr);
  HRESULT hr = 0;
  T_MercalliStreamCameraIntrinsic* CameraIntrinsic = nullptr;
  T_MercalliStreamSettingsB* SettingsB = nullptr;
  bool AnalysisRequired = false;
  bool FinishRequired = false; // "Finish" here likely refers to the smoothing/path finalization step after analysis.

  /* No Stream? */
  if(param.DataStream == nullptr)
    AnalysisRequired = true;
  else
  {
    /*
     * --- Re-Analysis if SourceCameraIntrinsic values changed ---
     */
    hr = MercalliGetStreamCameraIntrinsic(param.DataStream, &CameraIntrinsic); // get CameraIntrinsic
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGetStreamCameraIntrinsic failed\n");
      return hr;
    }

    // Analysis required if the source Cam is changed
    if(!IsEqual(&CameraIntrinsic->SourceCameraIntrinsic, &param.SourceCameraIntrinsic))
      AnalysisRequired = true;

    // Re-finish required if the target Cam is changed
    if(!IsEqual(&CameraIntrinsic->TargetCameraIntrinsic, &param.TargetCameraIntrinsic))
      FinishRequired = true;


    /*
     * --- Re-Analysis or Re-finish by Settings ---
     */
    hr = MercalliGetStreamSettingsB(param.DataStream, &SettingsB); // get SettingsB
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliGetStreamSettingsB failed\n");
      return hr;
    }

    // Status by helper functions
    if(MustRescan(&SettingsB->CurrentSettings, &param.Settings))
      AnalysisRequired = true;
    if(MustSmoothCalc(&SettingsB->CurrentSettings, &param.Settings))
      FinishRequired = true;
  }


  *pAnalysisRequired = AnalysisRequired;
  *pFinishRequired   = FinishRequired || AnalysisRequired;
  return hr;
}
#pragma endregion

#pragma region VideoStabilizationApplySettings
/**
* @brief Applies the current stabilization settings to the Global Motion Path (GMP),
* which typically involves re-calculating the smooth camera path, and then
* updates the Mercalli data stream with the new state.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationApplySettings(CVideoStabilizationParam& param)
{
  HRESULT hr;

  // set Cams (Source and Target intrinsics in GMP)
  hr = VideoStabilizationSetCam(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoStabilizationSetCam failed\n");
    return hr;
  }

  if(param.Verbose)
    printf("Call MercalliGmpApplySettings\n");

  // apply settings to GMP (this is the "smoothing" or "path finalization" step)
  hr = MercalliGmpApplySettings
  (
    param.GMP,
    param.SupportVariation ? FinPathFlags_SupportVariation : 0,
    &param.Settings,
    &param.AdjSettings // Output for adjusted settings (e.g., after auto-variation)
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGmpApplySettings failed\n");
    return hr;
  }

  // create/update Data-Stream with the new state from GMP.
  hr = UpdateMercalliDataStream(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "UpdateMercalliDataStream failed\n");
    return hr;
  }

  return hr;
}
#pragma endregion

#pragma region VideoStabilizationQuickApplySettings
/**
* @brief Quickly applies view-only settings.
* This updates target camera intrinsics in the GMP (if GMP exists)
* and updates settings directly in the existing DataStream.
* This is for changes that don't require re-analysis or re-smoothing of the camera path.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationQuickApplySettings(CVideoStabilizationParam& param)
{
  HRESULT hr = 0;
  T_MercalliStreamCameraIntrinsic* CameraIntrinsic = nullptr;

  /* Update Target-CameraIntrinsic if GMP is available */
  if(param.GMP != nullptr)
  {
    if(param.Verbose)
      printf("Call MercalliSetCameraIntrinsic\n");

    // set Target-CameraIntrinsic to GMP
    hr = MercalliSetCameraIntrinsic(param.GMP, CamIticLev_Target, &param.TargetCameraIntrinsic);
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliSetCameraIntrinsic failed\n");
      return hr;
    }
  }

  // Remember Settings in the DataStream
  hr = UpdateMercalliDataStreamSettingsB(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "UpdateMercalliDataStreamSettingsB failed\n");
    return hr;
  }

  // Update TargetCameraIntrinsic in the DataStream
  hr = MercalliGetStreamCameraIntrinsic(param.DataStream, &CameraIntrinsic); // get CameraIntrinsic
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetStreamCameraIntrinsic failed\n");
    return hr;
  }

  //--- set
  CameraIntrinsic->TargetCameraIntrinsic = param.TargetCameraIntrinsic;

  return hr;
}
#pragma endregion

#pragma region VideoStabilizationUpdate
/**
* @brief Main update function for video stabilization.
* It determines if re-analysis or re-application of settings is needed.
* If so, it performs those steps. Otherwise, it applies quick settings.
* @param param The video stabilization parameters.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationUpdate(CVideoStabilizationParam& param)
{
  HRESULT hr;
  bool AnalysisRequired, ApplyRequired; // ApplyRequired implies smoothing/finishing.

  // get requirements
  hr = VideoStabilizationUpdateRequirement(param, &AnalysisRequired, &ApplyRequired);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoStabilizationUpdateRequirement failed\n");
    return hr;
  }

  if(AnalysisRequired)
  {
    // perform analysis
    hr = VideoStabilizationAnalysis(param);
    if(FAILED(hr))
    {
      fprintf(stderr, "VideoStabilizationAnalysis failed\n");
      return hr;
    }
    // After analysis, ApplyRequired should be true (as set by VideoStabilizationUpdateRequirement).
  }

  if(ApplyRequired) // True if AnalysisRequired was true, or if only smoothing was needed.
  {
    // perform apply (smoothing and updating DataStream)
    hr = VideoStabilizationApplySettings(param);
    if(FAILED(hr))
    {
      fprintf(stderr, "VideoStabilizationApplySettings failed\n");
      return hr;
    }

    if(!param.UsePref) // If not in performance test mode, print current DataStream state.
      PrintMercalliDataStream(param);
  }
  else // Neither analysis nor full apply (smoothing) is required.
  {
    // use changes (apply view-only settings quickly)
    hr = VideoStabilizationQuickApplySettings(param);
    if(FAILED(hr))
    {
      fprintf(stderr, "VideoStabilizationQuickApplySettings failed\n");
      return hr;
    }
  }

  return hr;
}
#pragma endregion

#pragma region VideoStabilizationRenderFrame
/**
* @brief Renders a single stabilized frame using the Mercalli data stream.
* @param param The video stabilization parameters.
* @param frameNumber The original media frame number.
* @param field The field type of the source frame.
* @param pSourceFrame Pointer to the source video frame.
* @param pTargetFrame Pointer to the target video frame buffer.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationRenderFrame(
  CVideoStabilizationParam& param,
  SINT64       frameNumber,
  field_type   field,
  IVideoFrame* pSourceFrame,
  IVideoFrame* pTargetFrame)
{
  assert(pSourceFrame != nullptr && pTargetFrame != nullptr);
  HRESULT hr = 0;
  T_MercalliRenderFrameParam RenderParam{};
  CVideoFrameInfo SourceFrameInfo, TargetFrameInfo;

  // get source and target frame-info
  hr = pSourceFrame->GetInfo(&SourceFrameInfo);
  if(SUCCEEDED(hr))
    hr = pTargetFrame->GetInfo(&TargetFrameInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "GetInfo failed\n");
    return E_FAIL;
  }

  // set RenderParam
  RenderParam.Extra.Field = field;
  RenderParam.Src   = SourceFrameInfo;
  RenderParam.Tar   = TargetFrameInfo;

  //---
  //if(param.Verbose)
  //  printf("Call MercalliStreamRenderFrame: %" PRId64 "\n", frameNumber);

   /* Convert media frame number to Mercalli's internal 0-based frame/field index.
    * See also \ref VideoStabilizationAnalysis (Scan-FrameNumber from VideoReadFrameNumber)
    */
  SINT64 StabiFrameNumber = MercalliFrameNumberFromMediaFrame(param, frameNumber);

  // Render stabilized Frame using the DataStream
  hr = MercalliStreamRenderFrame(param.DataStream, StabiFrameNumber, &RenderParam);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliStreamRenderFrame failed: %" PRId64 " FrameNumber\n", StabiFrameNumber);
    return hr;
  }

  return hr;
}
#pragma endregion

#pragma region VideoStabilizationExport
/**
* @brief Exports the stabilized video by processing each frame:
* reading from decoder, rendering stabilized version, and writing to encoder.
* @param param The video stabilization parameters, including decoder and encoder.
* @return HRESULT indicating success or failure.
*/
HRESULT VideoStabilizationExport(CVideoStabilizationParam& param)
{
  assert(param.FrameStep > 0 && param.SourceStartFrameNumber < param.SourceEndFrameNumber);
  HRESULT hr = 0;
  SINT32 FrameNumber;
  #if USE_GetPerformanceTime
  FLOAT64 Pref = 0;
  #endif

#if defined(OS_MACOS10) || defined(OS_LINUX)
  const format_type Format = FMT_BGR8; // Quicktime often prefers RGB.
#else
  const format_type Format = FMT_VUY8; // Common YUV format.
#endif
  T_MercalliStreamMediaInfo* MediaInfo;
  SINT32 FrameCount = param.SourceEndFrameNumber - param.SourceStartFrameNumber;

  /* Video-Decoder and Video-Encoder already created
   * by Caller.
   */

  // get Media info from data-stream
  hr = MercalliGetStreamMediaInfo(param.DataStream, &MediaInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetStreamMediaInfo failed\n");
    return hr;
  }

  // check: Source-Video option match data-stream option (from analysis)
  // Ensures consistency between current source media info and what was used for analysis.
  if(param.SourceMediaInfo.FieldOrder != MediaInfo->FieldOrder)
  {
    fprintf(stderr, "FieldOrder does not match Input-Video (used in analysis)\n");
    return E_FAIL;
  }

  if(param.UsePref)
    printf("Start Video Export\n");

  #if USE_GetPerformanceTime
  Pref = GetPerformanceTime();
  #endif

  // for each frame
  for(FrameNumber = param.SourceStartFrameNumber; FrameNumber < param.SourceEndFrameNumber; FrameNumber += param.FrameStep)
  {
    IVideoFrame* SourceFrame = nullptr;
    IVideoFrame* TargetFrame = nullptr;
    CVideoFrameInfo FrameInfo; // Used to get source frame details for creating target.

    // read source frame
    hr = param.VideoDecoder->ReadFrame
    (
      &SourceFrame,
      FrameNumber,
      param.SourceMediaInfo.Width,
      param.SourceMediaInfo.Height,
      Format,
      param.FrameType
    );
    if(FAILED(hr))
    {
      fprintf(stderr, "ReadFrame %d failed\n", FrameNumber);
      return hr;
    }

    // get source frame info (to determine dimensions/PAR for target frame if not encoding)
    if(SUCCEEDED(hr))
    {
      hr = SourceFrame->GetInfo(&FrameInfo);
      if(FAILED(hr))
        fprintf(stderr, "GetInfo %d failed\n", FrameNumber);
    }
    // CRITICAL: If GetInfo fails, SourceFrame is leaked. Should be released before returning or breaking.

    // create target frame (frame-buffer)
    if(SUCCEEDED(hr))
    {
      if(param.VideoEncoder != nullptr) // If encoding, let encoder create its preferred frame.
        hr = param.VideoEncoder->CreateVideoFrame(&TargetFrame, Format, param.FrameType);
      else // If not encoding (e.g., preview), create a generic frame.
        hr = CreateVideoFrame(&TargetFrame, FrameInfo.Width, FrameInfo.Height, FrameInfo.PAR, FrameInfo.Format, param.FrameType);
      if(FAILED(hr))
        fprintf(stderr, "CreateVideoFrame %d failed\n", FrameNumber);
    }

    if(!param.Verbose && !param.UsePref) // Progress display.
      printf("\rExport: %d%%  ", (int)(100*(FrameNumber-param.SourceStartFrameNumber) / FrameCount) );

    if(!param.UsePref)
      fflush(stdout);

    // render stabilized frame
    if(SUCCEEDED(hr))
    {
      hr = VideoStabilizationRenderFrame
      (
        param,
        FrameNumber,
        param.SourceMediaInfo.FieldOrder,
        SourceFrame,
        TargetFrame
      );
      if(FAILED(hr))
        fprintf(stderr, "VideoStabilizationRenderFrame %d failed\n", FrameNumber);
    }

    // encode stabilized frame
    if(SUCCEEDED(hr) && param.VideoEncoder != nullptr)
    {
      hr = param.VideoEncoder->WriteFrame(TargetFrame);
      if(FAILED(hr))
        fprintf(stderr, "WriteFrame %d failed\n", FrameNumber);
    }


    // clean up
    if(SourceFrame != nullptr)
      SourceFrame->Release();
    if(TargetFrame != nullptr)
      TargetFrame->Release();

    if(FAILED(hr))
      break;
  }

  #if USE_GetPerformanceTime
  Pref = GetPerformanceTime() - Pref;

  if(param.UsePref)
  {
    SINT32 Frames = param.SourceEndFrameNumber- param.SourceStartFrameNumber;
    printf("+++Pref VideoStabilizationExport %0.1ff/s  %d Frames\n\n", Frames / Pref, Frames);
  }
  else
  #endif
  {
    if(!param.Verbose)
      printf("\rExport finished   \n");
  }
  return hr;
}
#pragma endregion

#pragma region VideoStabilizationPrintCharts
/**
* @brief Prints stabilization chart data (e.g., translation X, Y) for each frame
* from the smoothed frame matrix in the DataStream.
* @param param The video stabilization parameters containing the DataStream.
* @return HRESULT indicating success or failure (typically S_OK unless stream access fails).
*/
HRESULT VideoStabilizationPrintCharts(CVideoStabilizationParam& param)
{
  HRESULT hr = 0;
  SINT32 FrameNumber;

  T_MercalliStreamMediaInfo* MediaInfo = nullptr;
  T_MercalliStreamFrameMatrixKey* FrameMatrixKey = nullptr;
  //SINT32 FrameCount = param.SourceEndFrameNumber - param.SourceStartFrameNumber;

  // get Media info from data-stream (Not strictly needed for this function's logic, but called)
  hr = MercalliGetStreamMediaInfo(param.DataStream, &MediaInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetStreamMediaInfo failed\n");
    return hr;
  }

  hr = MercalliGetStreamSmoothFrameMatrix(param.DataStream, &FrameMatrixKey);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetStreamSmoothFrameMatrix failed\n");
    return hr;
  }

  // for each frame
  for(FrameNumber = param.SourceStartFrameNumber; FrameNumber < param.SourceEndFrameNumber; FrameNumber += param.FrameStep)
  {
    SINT32 FrameIndex = FrameNumber - param.SourceStartFrameNumber;
    if(FrameIndex >= FrameMatrixKey->Count)
    {
      printf("%4d: ??? \n", FrameNumber);
    }
    else
    {
      const T_MercalliStreamFrameMatrix& FM = FrameMatrixKey->Items[FrameIndex];
      const int MaxItems = 2;
      char ValText[MaxItems][24];
      FLOAT32 Val[MaxItems] =
      {
        FM.T[0],
        FM.T[1],
      };
      
      // This formatting logic is a bit convoluted.
      // Goal seems to be right-aligned fixed-width float.
      // Example: "   0.1" (7 chars if space before first digit)
      // Or " -12.3"
      for(int i = 0; i < MaxItems; ++i)
      {
        snprintf(ValText[i], sizeof(ValText[i]), "    %0.1f", Val[i]);
        size_t Len = strlen(ValText[i]);
        size_t Mv = Len-7;
        memmove(ValText[i], &ValText[i][Mv], Len-Mv);
        ValText[i][Len - Mv] = 0;
      }

      printf("%4d: T:%s; %s \n"
        , FrameNumber
        , ValText[0]
        , ValText[1]
      );
    }

  }

  printf("\n");
  return hr;
}

#pragma endregion

