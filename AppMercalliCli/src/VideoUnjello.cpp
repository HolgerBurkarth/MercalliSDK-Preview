/*******************************************************************
$CRT 18 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>VideoUnjello.cpp<< 06 Mai 2015  16:20:36 - (c) proDAD
*******************************************************************/
#include "stdafx.h"

#pragma region CVideoUnjelloParam Construction
CVideoUnjelloParam::~CVideoUnjelloParam()
{
  if(Unjello != nullptr)
  {
    if(Verbose)
      printf("Call MercalliDeleteUnjello()\n");
    MercalliDeleteUnjello(Unjello);
    Unjello = nullptr;
  }

  if(VideoDecoder != nullptr)
    VideoDecoder->Release();
  if(VideoEncoder != nullptr)
    VideoEncoder->Release();
}

CVideoUnjelloParam::CVideoUnjelloParam()
{
}

#pragma endregion

#pragma region UnjelloProgress (Customer)
/**
* @brief Callback function invoked by Mercalli SDK during Unjello analysis to report progress.
*
* This function retrieves custom data (CVideoUnjelloParam) to access application-specific state
* if needed. It prints progress to the console.
* The user can add custom logic here, for example, to update a GUI progress bar.
* Returning E_ABORT from this callback can cancel the ongoing analysis.
*
* @param pUJ Pointer to the T_Unjello instance.
* @param currentFrameNumber The current frame number being processed in the analysis.
* @param startFrameNumber The starting frame number of the analysis range.
* @param endFrameNumber The ending frame number of the analysis range.
* @return HRESULT S_OK to continue, E_ABORT to cancel, or other error code.
*/
STDCALLBACK(UnjelloProgress)(
  const T_Unjello* pUJ,
        SINT64 currentFrameNumber,
        SINT64 startFrameNumber,
        SINT64 endFrameNumber)
{
  HRESULT hr = 0;
  CVideoUnjelloParam* Param{};

  hr = MercalliGetUnjelloCustomData(pUJ, &Param);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetUnjelloCustomData failed\n");
    return hr;
  }

  printf("\rAnalyse: %d%%  ", (int)(100*(currentFrameNumber - startFrameNumber) / (endFrameNumber - startFrameNumber) ) );
  fflush(stdout);
  

  // TODO: your code 

  /* Abort Analysis by return E_ABORT */
  return hr;
}
#pragma endregion

#pragma region UnjelloFrame (Customer)
/**
* @brief Callback function invoked by Mercalli SDK to request a video frame for Unjello processing.
*
* This function retrieves custom data (CVideoUnjelloParam) to access the video decoder.
* It reads the requested frame using the application's IVideoDecoder,
* retrieves its CVideoFrameInfo, and then pushes this frame data back to the SDK
* using the provided `pushFrame` callback.
*
* @param pUJ Pointer to the T_Unjello instance.
* @param pArgs Arguments for the frame request (frame number, required dimensions, format).
* @param pushFrame Function pointer to push the acquired frame data back to the SDK.
* @param pPushFrameHandle Opaque handle to be passed to the `pushFrame` function.
* @return HRESULT S_OK on success, or an error code if frame acquisition/pushing fails.
*/
STDCALLBACK(UnjelloFrame)(
        T_Unjello*               pUJ,
  const T_UnjelloFrameArguments* pArgs,
        UnjelloPushFrameCallback pushFrame,
        void*                    pPushFrameHandle)
{
  HRESULT hr = 0;
  CVideoUnjelloParam* Param;
  IVideoFrame* Frame = nullptr;
  CVideoFrameInfo Info;
  T_UnjelloPushFrameParam PushFrameParam{};

  hr = MercalliGetUnjelloCustomData(pUJ, &Param);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliGetUnjelloCustomData failed\n");
    return hr;
  }
  if(Param->Verbose)
    printf("Call MercalliGetUnjelloCustomData() before\n");

  if(Param->Verbose)
    printf("Get Frame %" PRId64 "\n", pArgs->FrameNumber);

  // Read Source Frame
  hr = Param->VideoDecoder->ReadFrame
  (
    &Frame,
    pArgs->FrameNumber,
    pArgs->Width,
    pArgs->Height,
    (format_type)pArgs->Format
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "ReadFrame failed\n");
    return hr;
  }

  // get Frame info
  hr = Frame->GetInfo(&Info);
  if(FAILED(hr))
    fprintf(stderr, "GetInfo failed\n");

  if(SUCCEEDED(hr))
  {
    PushFrameParam.Image = Info;

    if(Param->Verbose)
      printf("Call (*pushFrame)()\n");

    hr = (*pushFrame)(pUJ, pPushFrameHandle, pArgs->FrameNumber, &PushFrameParam);
    if(FAILED(hr))
      fprintf(stderr, "pushFrame failed\n");
  }

  // Clean-up
  if(Frame != nullptr)
    Frame->Release();
  return hr;
}
#pragma endregion

#pragma region VideoUnjelloSetup
/**
* @brief Configures the T_Unjello instance with essential callbacks and custom data.
* This includes setting a custom data pointer (CVideoUnjelloParam instance) for access
* within callbacks, and registering the UnjelloFrame callback for frame provision.
* @param param The video Unjello parameters containing the T_Unjello instance.
* @return HRESULT S_OK on success, or an error code if setup fails.
*/
HRESULT VideoUnjelloSetup(CVideoUnjelloParam& param)
{
  assert(param.Unjello != nullptr);
  HRESULT hr = 0;

  if(param.Verbose)
    printf("Call MercalliSetUnjelloCustomData()\n");

  hr = MercalliSetUnjelloCustomData(param.Unjello, (UINT_PTR)&param);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloCustomData failed\n");
    return hr;
  }


  if(param.Verbose)
    printf("Call MercalliSetUnjelloFrameCallback()\n");

  hr = MercalliSetUnjelloFrameCallback(param.Unjello, &UnjelloFrame);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloFrameCallback failed\n");
    return hr;
  }


  return hr;
}
#pragma endregion

#pragma region VideoUnjelloUpdate
/**
* @brief Updates various Unjello processing parameters and triggers analysis if necessary.
* Sets the frame range, render system, processing method, and shutter speed settings.
* Then calls VideoUnjelloAnalysis to perform shutter speed estimation if configured.
* @param param The video Unjello parameters.
* @return HRESULT S_OK on success, or an error code if any update step fails.
*/
HRESULT VideoUnjelloUpdate(CVideoUnjelloParam& param)
{
  assert(param.Unjello != nullptr);
  HRESULT hr = 0;

  if(param.Verbose)
    printf("Call MercalliSetUnjelloFrameNumberRange()\n");

  // set work-area (processing range for Unjello)
  hr = MercalliSetUnjelloFrameNumberRange
  (
    param.Unjello,
    param.SourceStartFrameNumber,
    param.SourceEndFrameNumber
  );
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloFrameNumberRange failed\n");
    return hr;
  }

  // Set Render-System
  hr = MercalliSetUnjelloRenderSystem(param.Unjello, param.RenderSystem);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloRenderSystem failed\n");
    return hr;
  }

  // Set Method if defined
  if(param.Method != UJMthTyp_Unknown)
  {
    hr = MercalliSetUnjelloMethod(param.Unjello, param.Method);
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliSetUnjelloMethod failed\n");
      return hr;
    }
  }

  /* enable/disable Shutter-Speed estimation
   * see also VideoUnjelloAnalysis()
   */
  hr = MercalliSetUnjelloEnableAutoShutterSpeed(param.Unjello, param.AutoShutterSpeed);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloEnableAutoShutterSpeed failed\n");
    return hr;
  }

  // explicit set Shutter-Speed, will be used if SetUnjelloEnableAutoShutterSpeed(false)
  hr = MercalliSetUnjelloShutterSpeed(param.Unjello, param.ShutterSpeed);
  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliSetUnjelloShutterSpeed failed\n");
    return hr;
  }

  /* Analyze frames if required */
  hr = VideoUnjelloAnalysis(param);
  if(FAILED(hr))
  {
    fprintf(stderr, "VideoUnjelloAnalysis failed\n");
    return hr;
  }

  return hr;
}
#pragma endregion

#pragma region VideoUnjelloAnalysis
/**
* @brief Performs sensor shutter speed analysis if configured and required by the SDK.
* This uses the Mercalli SDK's estimation capabilities. The UnjelloProgress callback
* will be invoked during this process if registered.
* @param param The video Unjello parameters.
* @return HRESULT S_OK on success or if analysis is not required/aborted,
*         or an error code if a critical step fails.
*/
HRESULT VideoUnjelloAnalysis(CVideoUnjelloParam& param)
{
  assert(param.Unjello != nullptr);
  HRESULT hr = 0;
  BOOL EstimateRequired = FALSE;

  /* Estimate sensor read-out speed if SetUnjelloEnableAutoShutterSpeed(true) */
  if(param.EstimateShutterSpeed && param.AutoShutterSpeed)
  {
    hr = MercalliEstimateUnjelloShutterSpeedRequired(param.Unjello, &EstimateRequired);
    if(FAILED(hr))
    {
      fprintf(stderr, "MercalliEstimateUnjelloShutterSpeedRequired failed\n");
      return hr;
    }

    if(EstimateRequired)
    {
      if(param.Verbose)
        printf("Call MercalliEstimateUnjelloShutterSpeed()\n");

      printf("Perform: Sensor Shutter Speed Analysis\n");

      /* Analyse from SourceStartFrameNumber to SourceEndFrameNumber. 
       * see SetUnjelloFrameNumberRange()
       */
      hr = MercalliEstimateUnjelloShutterSpeed(param.Unjello, &UnjelloProgress);
      if(hr == E_ABORT)
      {
        printf("MercalliEstimateUnjelloShutterSpeed aborted\n");
      }
      else if(FAILED(hr))
      {
        fprintf(stderr, "MercalliEstimateUnjelloShutterSpeed failed\n");
        return hr;
      }
      else // if SUCCEEDED
      {
        // get estimated avg. Speed
        FLOAT32 AvgSpeed;
        hr = MercalliGetUnjelloAverageShutterSpeed(param.Unjello, &AvgSpeed);
        if(FAILED(hr))
        {
          fprintf(stderr, "MercalliGetUnjelloAverageShutterSpeed failed\n");
          return hr;
        }

        printf("\rAverage Shutter Speed = %f\n", AvgSpeed);
      }
    } // if(EstimateRequired)
  }

  return hr;
}
#pragma endregion

#pragma region VideoUnjelloRenderFrame
/**
* @brief Renders a single Unjello-processed (de-warped) frame.
* For latency-based render systems, it pre-loads the source frame.
* For other systems (like UJRSYS_SyncPreload), the UnjelloFrame callback would have
* already supplied necessary frames during an earlier phase or synchronously.
* @param param The video Unjello parameters.
* @param frameNumber The media frame number to render.
* @param field The field type of the frame.
* @param pTargetFrame The video frame buffer to render into.
* @return HRESULT S_OK on success, or an error code on failure.
*/
HRESULT VideoUnjelloRenderFrame(
  CVideoUnjelloParam& param,
  SINT64       frameNumber,
  field_type   field,
  IVideoFrame* pTargetFrame)
{
  assert(pTargetFrame != nullptr);
  HRESULT hr = 0;
  T_UnjelloRenderFrameParam RenderParam{};
  CVideoFrameInfo SourceFrameInfo, TargetFrameInfo;
  IVideoFrame* SourceFrame = nullptr;

  // get target frame-info
  hr = pTargetFrame->GetInfo(&TargetFrameInfo);
  if(FAILED(hr))
  {
    fprintf(stderr, "GetInfo failed\n");
    return E_FAIL;
  }

  // set RenderParam
  RenderParam.Field = field;
  RenderParam.Image = TargetFrameInfo;


  // support Latency Method by pre-loading source frame
  if(  param.RenderSystem == UJRSYS_Latency1 
    || param.RenderSystem == UJRSYS_Latency2
    || param.RenderSystem == UJRSYS_Latency3)
  {
    // get
    hr = param.VideoDecoder->ReadFrame
    (
      &SourceFrame,
      frameNumber,
      TargetFrameInfo.Width,
      TargetFrameInfo.Height,
      (format_type)TargetFrameInfo.Format
    );
    if(FAILED(hr))
    {
      fprintf(stderr, "ReadFrame failed\n");
      return E_FAIL;
    }

    // get target frame-info
    hr = SourceFrame->GetInfo(&SourceFrameInfo);
    if(FAILED(hr))
    {
      SourceFrame->Release();
      fprintf(stderr, "GetInfo failed\n");
      return E_FAIL;
    }

    RenderParam.Src = SourceFrameInfo;
  }


  //---
  if(param.Verbose)
    printf("Call MercalliUnjelloRenderFrame: %" PRId64 "\n", frameNumber);

  // Render unjello Frame
  hr = MercalliUnjelloRenderFrame(param.Unjello, frameNumber, &RenderParam);

  // Release Source Frame
  if(SourceFrame != nullptr)
    SourceFrame->Release();

  if(FAILED(hr))
  {
    fprintf(stderr, "MercalliUnjelloRenderFrame failed\n");
    return hr;
  }

  return hr;
}
#pragma endregion

#pragma region VideoUnjelloExport
/**
* @brief Exports the Unjello-processed video.
* For each frame in the specified range, it creates a target buffer,
* renders the Unjello-processed version into it, and then writes it to the video encoder.
* @param param The video Unjello parameters, including decoder and encoder.
* @return HRESULT S_OK on success, or an error code on failure.
*/
HRESULT VideoUnjelloExport(CVideoUnjelloParam& param)
{
  HRESULT hr = 0;
  SINT32 FrameNumber;
#if defined(OS_MACOS10)
  // Quicktime only works with RGB
  const format_type Format = FMT_BGR8;
#else
  //const format_type Format = FMT_VUY8;
  const format_type Format = FMT_BGR8;
#endif
  SINT32 FrameCount = param.SourceEndFrameNumber - param.SourceStartFrameNumber;

  /* Video-Decoder and Video-Encoder already created
   * by Caller.
   */


  // for each frame
  for(FrameNumber = param.SourceStartFrameNumber; FrameNumber < param.SourceEndFrameNumber; ++FrameNumber)
  {
    IVideoFrame* TargetFrame = nullptr;

    // create target frame (frame-buffer)
    hr = CreateVideoFrame
    (
      &TargetFrame,
      param.SourceMediaInfo.Width,
      param.SourceMediaInfo.Height,
      param.SourceMediaInfo.PAR,
      Format
    );
    if(FAILED(hr))
    {
      fprintf(stderr, "CreateVideoFrame failed\n");
      return hr;
    }

    if(!param.Verbose)
      printf("\rExport: %d%%  ", (int)(100*(FrameNumber-param.SourceStartFrameNumber) / FrameCount) );

    fflush(stdout);
    
    // render unjello-ed frame
    if(SUCCEEDED(hr))
    {
      hr = VideoUnjelloRenderFrame
      (
        param,
        FrameNumber,
        param.SourceMediaInfo.FieldOrder,
        TargetFrame
      );
      if(FAILED(hr))
        fprintf(stderr, "VideoUnjelloRenderFrame %d failed\n", FrameNumber);
    }

    // encode unjello-ed frame
    if(SUCCEEDED(hr))
    {
      hr = param.VideoEncoder->WriteFrame(TargetFrame);
      if(FAILED(hr))
        fprintf(stderr, "WriteFrame %d failed\n", FrameNumber);
    }


    // clean up
    if(TargetFrame != nullptr)
      TargetFrame->Release();

    if(FAILED(hr))
      break;
  }

  if(!param.Verbose)
    printf("\rExport finished   \n");
  return hr;
}
#pragma endregion

#pragma region PrintUnjelloOptions
/**
* @brief Prints the current Unjello configuration options to the console.
* Retrieves method, auto shutter speed state, shutter speed value, and frame range
* from the T_Unjello instance.
* @param param The video Unjello parameters containing the T_Unjello instance.
*/
void PrintUnjelloOptions(CVideoUnjelloParam& param)
{
  HRESULT hr;
  UINT32 Method = 0;
  BOOL   EnableAutoShutterSpeed = 0;
  FLOAT32 ShutterSpeed = 0;
  SINT64 StartFrameNumber = 0, EndFrameNumber = 0;

  hr = MercalliGetUnjelloMethod(param.Unjello, &Method);
  if(FAILED(hr))
    fprintf(stderr, "MercalliGetUnjelloMethod failed\n");

  hr = MercalliGetUnjelloEnableAutoShutterSpeed(param.Unjello, &EnableAutoShutterSpeed);
  if(FAILED(hr))
    fprintf(stderr, "MercalliGetUnjelloEnableAutoShutterSpeed failed\n");

  hr = MercalliGetUnjelloShutterSpeed(param.Unjello, &ShutterSpeed);
  if(FAILED(hr))
    fprintf(stderr, "MercalliGetUnjelloShutterSpeed failed\n");

  hr = MercalliGetUnjelloFrameNumberRange(param.Unjello, &StartFrameNumber, &EndFrameNumber);
  if(FAILED(hr))
    fprintf(stderr, "MercalliGetUnjelloFrameNumberRange failed\n");


  printf(
      "Unjello Options\n"
      "  Method:            %d\n"
      "  Auto ShutterSpeed: %s\n"
      "  ShutterSpeed:      %f\n"
      "  StartFrameNumber:  %" PRId64 "\n"
      "  EndFrameNumber:    %" PRId64 "\n"
    , Method
    , EnableAutoShutterSpeed ? "ON":"OFF"
    , ShutterSpeed
    , StartFrameNumber
    , EndFrameNumber
  );

}
#pragma endregion
