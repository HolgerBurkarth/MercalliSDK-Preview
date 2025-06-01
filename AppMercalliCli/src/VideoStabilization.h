#ifndef __MERCALLICLI_VIDEOSTABILIZATION_H
#define __MERCALLICLI_VIDEOSTABILIZATION_H
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>VideoStabilization.h<< 15 Okt 2024  10:00:36 - (c) proDAD
*******************************************************************/

/*! \brief Video stabilization for smoothing camera motion paths.
 */

#pragma region Flowchart
 /*!
 ==========================================================
 =           Stabilization Process Overview               =
 ==========================================================

                    /-------------/
                   /  Open Video /
                  /-------------/
                          |
                          +------------------<------+
                          v                         |
              +---------------------------+         |
              ||  Stabilization Process  ||         |
              +---------------------------+         |
                 |                  |               |
                 v                  v               ^
   /---------------------/  +------------------+    |
  /  Playback Process   /   || Export Process ||    |
 /---------------------/    +------------------+    |
           v                        v               |
           |                        |               |
           +------------>-----------+----->---------+





 ===============================================================
 =                  Stabilization Process                      =
 ===============================================================

             +------<--------+
             v               |
    +-------------------+    |
    ||  Configuration  ||    |
    +-------------------+    |
             |               |
             v               |
    +-------------------+    |
    ||    Update       ||    |
    +-------------------+    ^
             |               |
 +----->-----+               |
 |           v               |
 |   +------------------+    |
 |   |  Render Frame    |    |
 |   +-------+----------+    |
 |           v               |
 +---<------/ \---->---------+




 ===============================================================
 =                  Configuration                              =
 ===============================================================

   +-------------------------+  Yes +--------------+
   | Video settings changed? |----->| Delete GMP   |
   | Such: FieldOrder, Rate, |      +--------------+
   | Size, Start-Endtime, ...|             |
   +-------------------------+             |
              |                            |
              v                            v
   +-------------------------+  Yes +--------------+
   | If GMP == NULL          |----->| Create GMP   |
   +-------------------------+      +--------------+
              |                            v
              +-------------<--------------+
              v
   +------------------------------+
   | Set Properties               |
   | to GMP or/and T_SettingsB    |
   |------------------------------|
   | Camera Intrinsic        < 1> |
   | Apply Stabi Profile     <16> |
   | T_SettingsB Value       < 3> |
   | etc.                    < 4> |
   +------------------------------+







 ===============================================================
 =                     Update < 9>                             =
 ===============================================================

  +-----------------------------+
  | What is to be updated? < 5> |
  |-----------------------------|      +----------------------+
  |          Analysis required? +----->|| Analyze Video < 6> ||
  |                             |      +----------------------+
  |                             |              v
  |                             |      +----------------------+
  |             Apply required? +----->| Apply Settings < 7>  |
  +-----------------------------+      +----------------------+
              |                                v
              v                                |
  +-----------------------------+              |
  | Apply instant Settings < 8> |              |
  +-----------------------------+              |
              |                                |
              +--------------<-----------------+
              |
              v





 ===============================================================
 =                  Analysis Video <6>                         =
 ===============================================================

 +-------------------------+
 | Update:                 |
 |- Camera Intrinsic  < 1> |
 |- Stabi Profile     <16> |
 |- T_SettingsB            |
 +-------------------------+
           |
           |              +------------------------------------+
           |              |      Analyze Video Frame           |
           |              |------------------------------------|
           |              |   +----------------------------+   |
           v           +----> |    Read Source Frame       |   |
  +-----------------+  ^  |   +----------------------------+   |
  |  For Each Frame +--:  |               v                    |
  +--------+--------+  ^  |   +----------------------------+   |
           |           |  |   |  Analyze Frame             |   |
           v           +----< |  <13>                      |   |
                          |   +----------------------------+   |
                          +------------------------------------+






 ===============================================================
 =                      Export Process <11>                    =
 ===============================================================

   /------------------/
  /  Create Encoder  /
 /------------------/
           |              +------------------------------------+
           |              |           Render Frame             |
           |              |------------------------------------|
           |              |   +----------------------------+   |
           |           +----> |    Read Source Frame       |   |
           |           |  |   +----------------------------+   |
           |           |  |                v                   |
           |           |  |   +----------------------------+   |
           v           |  |   | Create Target Frame Buffer |   |
  +-----------------+  ^  |   +----------------------------+   |
  |  For Each Frame +--:  |                v                   |
  +--------+--------+  ^  |   +----------------------------+   |
           |           |  |   |  Render stabilized Frame   |   |
           |           |  |   |  <12>                      |   |
           |           |  |   |                            |   |
           |           |  |   |       Source-Frame         |   |
           |           |  |   |            v               |   |
           |           |  |   |        Operation           |   |
           |           |  |   |            v               |   |
           |           |  |   |       Target-Frame         |   |
           |           |  |   +----------------------------+   |
           |           |  |                v                   |
           |           |  |   +----------------------------+   |
           |           +----< |    Encode Target Frame     |   |
           |              |   +----------------------------+   |
           v              +------------------------------------+
    /-----------------/
   / Close Encoder   /
  /-----------------/




 ==========================================================
 =                  Mercalli Data Stream                  =
 ==========================================================

 +--------------------------------------------------------+
 |                   Using DataStream                     |
 |                 ( GMP not required )                   |
 |--------------------------------------------------------|
 |                                                        |
 |  +--------------------------+ Yes                      |
 |  | if DataStream == NULL?   |--->----------+           |
 |  +--------------------------+              |           |
 |              | No                          |           |
 |              v                             v           |
 |  +--------------------------+ Yes +-----------------+  |
 |  | if Update required?  <5> |---->| Create / Update > > > > > > +
 |  +--------------------------+     | DataStream      |  |        v
 |              | No                 +-----------------+  |
 |              |                             v           |        v
 |              |                             |           |
 |              +-----------<-----------------+           |        v
 |              v                                         |
 |  +--------------------------+                          |        v
 |  | Render Frame <12>        |                          |
 |  +--------------------------+                          |        v
 |                                                        |
 +--------------------------------------------------------+        v

                                                                   v

 +--------------------------------------------------------+        v
 |               Create / Update DataStream               | < < <  +
 |                  ( GMP required )                      |
 |--------------------------------------------------------|
 |                                                        |
 |  +------------------------+ Yes  +-----------------+   |
 |  | if DataStream != NULL? |----->| Free DataStream |   |
 |  +----------+-------------+      | <15>            |   |
 |             | No                 +-----------------+   |
 |             |                             v            |
 |             |                             |            |
 |             +------------<----------------+            |
 |             v                                          |
 |  +------------------------------+                      |
 |  | Stabilization Process <9>    |                      |
 |  +------------------------------+                      |
 |             |                                          |
 |             v                                          |
 |  +------------------------------+                      |
 |  | Create and Update DataStream |                      |
 |  | <14>                         |                      |
 |  +------------------------------+                      |
 |                                                        |
 +--------------------------------------------------------+



 Footer:
 < 1>  MercalliSetCameraIntrinsic()
 < 3>  MercalliApplyProfile()
 < 4>  EnableGlobFrameRS(), etc.
 < 5>  see VideoStabilizationUpdateRequirement()
 < 6>  see VideoStabilizationAnalysis()
 < 7>  see VideoStabilizationApplySettings()
 < 8>  see VideoStabilizationQuickApplySettings()
 < 9>  see VideoStabilizationUpdate()
 <11>  see VideoStabilizationExport()
 <12>  see VideoStabilizationRenderFrame()
 <13>  see MercalliGmpScanImage()
 <14>  see UpdateMercalliDataStream()
 <15>  see MercalliFreeDataStream()
 <16>  see VideoStabilizationApplyStabiProfile()

  */
#pragma endregion

#pragma region CVideoStabilizationParam
/*! \brief Structure for configuring video stabilization parameters.
  */
struct CVideoStabilizationParam
{
  bool                  Verbose{};                 /*!< Enable verbose output */
  bool                  UsePref{};                 /*!< Use preferred settings */
  FrameType_Enum        FrameType{ FrameType_Standard }; /*!< Frame processing type */
  IVideoDecoder*        VideoDecoder{};            /*!< Source video decoder */
  IVideoEncoder*        VideoEncoder{};            /*!< Target video encoder */
  CMediaInfo            SourceMediaInfo{};         /*!< Source video metadata */
  SINT32                SourceStartFrameNumber{};  /*!< Start frame number */
  SINT32                SourceEndFrameNumber{};    /*!< End frame number */
  SINT32                FrameStep{ 1 };            /*!< Frame step increment */
  T_GlobalMotionPath*   GMP{};                     /*!< Global motion path */
  T_MercalliDataStream* DataStream{};              /*!< Mercalli data stream */
  SINT32                ProfileID{ 42 };           /*!< Stabilization profile ID (default: Intelli-Universal) */
  T_SettingsB           Settings{};                /*!< Stabilization settings */
  T_SettingsB           AdjSettings{};             /*!< Adjusted settings by \ref MercalliGmpApplySettings() */
  T_CameraIntrinsic     SourceCameraIntrinsic{};   /*!< Source camera intrinsic parameters */
  T_CameraIntrinsic     TargetCameraIntrinsic{};   /*!< Target camera intrinsic parameters */
  bool                  SupportVariation{ true };  /*!< Enable variation support */

  /*! \brief Destructor for CVideoStabilizationParam.
   */
  ~CVideoStabilizationParam();

  /*! \brief Constructor for CVideoStabilizationParam.
   */
  CVideoStabilizationParam();

  /*! \brief Enables or disables global frame rolling shutter compensation.
   * \note Call VideoStabilizationUpdate() to apply the change.
   * \note This is a fast process for full-frame CMOS distortions. Sub-frame distortions require Unjello.
   * \param value True to enable, false to disable
   * \return HRESULT indicating success (always 0)
   */
  HRESULT EnableGlobFrameRS(bool value)
  {
    Settings.ScanFlags &= ~ScanFlags_GlobFrameRS;
    if(value)
      Settings.ScanFlags |= ScanFlags_GlobFrameRS;
    return 0;
  }

  /*! \brief Enables or disables dynamic camera mode.
   * \note Call VideoStabilizationUpdate() to apply the change.
   * \note Disables automatic variation support when enabled.
   * \param value True to enable, false to disable
   * \return HRESULT indicating success (always 0)
   */
  HRESULT EnableDynamicCamera(bool value)
  {
    Settings.ViewFlags &= ~ViewFlags_DynamicCamera;
    if(value)
      Settings.ViewFlags |= ViewFlags_DynamicCamera;

    SupportVariation = false; /*!< Manual setup, disable automatic variation */
    return 0;
  }

  /*! \brief Sets the source camera intrinsic parameters.
   * \note Call VideoStabilizationUpdate() to apply the change.
   * \param value Camera intrinsic parameters
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetSourceCameraIntrinsic(const T_CameraIntrinsic& value)
  {
    SourceCameraIntrinsic = value;
    return 0;
  }

  /*! \brief Sets the target camera intrinsic parameters.
   * \note Call VideoStabilizationUpdate() to apply the change.
   * \param value Camera intrinsic parameters
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetTargetCameraIntrinsic(const T_CameraIntrinsic& value)
  {
    TargetCameraIntrinsic = value;
    return 0;
  }

  /*! \brief Sets the border mode for stabilization.
   * \note Disables automatic variation support.
   * \param value Border parameter (BorderParam_Enum)
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetBorderMode(BorderParam_Enum value)
  {
    Settings.BorderParam = value;
    SupportVariation = false; /*!< Manual setup, disable automatic variation */
    return 0;
  }

  /*! \brief Sets the translation smoothing factor.
   * \note Disables automatic variation support.
   * \param value Smoothing factor
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetTranslationSmoothing(float value)
  {
    Settings.TransFac = (SINT32)(value * 65536.0f);
    SupportVariation = false; /*!< Manual setup, disable automatic variation */
    return 0;
  }

  /*! \brief Sets the Z-rotation smoothing factor.
   * \note Disables automatic variation support.
   * \param value Smoothing factor
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetZRotationSmoothing(float value)
  {
    Settings.ZRotationFac = (SINT32)(value * 65536.0f);
    SupportVariation = false; /*!< Manual setup, disable automatic variation */
    return 0;
  }

  /*! \brief Sets the view scale factor.
   * \note Disables automatic variation support.
   * \param value Scale factor
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetViewScale(float value)
  {
    Settings.ViewScale = (SINT32)(value * 65536.0f);
    SupportVariation = false; /*!< Manual setup, disable automatic variation */
    return 0;
  }
};
#pragma endregion

#pragma region Functions
/*! \brief Exports a stabilized video by rendering each frame.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationExport(CVideoStabilizationParam& param);

/*! \brief Prints stabilization charts to visualize the process.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationPrintCharts(CVideoStabilizationParam& param);

/*! \brief Renders a stabilized frame.
 * \param param        Stabilization parameters
 * \param frameNumber  Frame index
 * \param field        Field type for interlaced video
 * \param pSourceFrame Source video frame
 * \param pTargetFrame Target video frame (input/output)
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationRenderFrame(
  /* in  */ CVideoStabilizationParam& param,
  /* in  */ SINT64       frameNumber,
  /* in  */ field_type   field,
  /* in  */ IVideoFrame* pSourceFrame,
  /* i/o */ IVideoFrame* pTargetFrame);

/*! \brief Updates variable properties and performs analysis if required.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationUpdate(CVideoStabilizationParam& param);

/*! \brief Quickly applies view-only settings to the GMP and updates the DataStream.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationQuickApplySettings(CVideoStabilizationParam& param);

/*! \brief Applies settings to the GMP and updates the DataStream.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationApplySettings(CVideoStabilizationParam& param);

/*! \brief Determines the update requirements for stabilization.
 * \param param             Stabilization parameters
 * \param pAnalysisRequired Output: True if analysis is required
 * \param pFinishRequired   Output: True if finishing is required
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationUpdateRequirement(
  /* in  */ CVideoStabilizationParam& param,
  /* out */ bool* pAnalysisRequired,
  /* out */ bool* pFinishRequired);

/*! \brief Updates the DataStream by recreating and storing all data.
 * \note Similar to serialization.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT UpdateMercalliDataStream(CVideoStabilizationParam& param);

/*! \brief Sets the settings and profile in the DataStream.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT UpdateMercalliDataStreamSettingsB(CVideoStabilizationParam& param);

/*! \brief Performs video analysis for stabilization.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationAnalysis(CVideoStabilizationParam& param);

/*! \brief Sets the source and target camera intrinsic parameters.
 * \param param Stabilization parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationSetCam(CVideoStabilizationParam& param);

/*! \brief Applies a stabilization profile.
 * \param param Stabilization parameters
 * \param id    Profile ID:
 *   - 42: Turbo (includes FinPathFlags_SupportVariation)
 *   - 44: AI (includes FinPathFlags_SupportVariation)
 *   - 45: AI3D (includes FinPathFlags_SupportVariation)
 *   - 46: Hyper-lapse (up to 10x acceleration)
 * \return HRESULT indicating success or failure
 */
HRESULT VideoStabilizationApplyStabiProfile(CVideoStabilizationParam& param, SINT32 id = 42);

/*! \brief Prints DataStream options to the console.
 * \param param Stabilization parameters
 */
void PrintMercalliDataStream(CVideoStabilizationParam& param);

/*! \brief Converts a source video frame number to a Mercalli frame number.
 * \note Mercalli frame numbers start at 0.
 * \param param           Stabilization parameters
 * \param mediaFrameNumber Source video frame number
 * \return Mercalli frame number
 */
__STATIC_INLINE SINT64 MercalliFrameNumberFromMediaFrame(const CVideoStabilizationParam& param, SINT64 mediaFrameNumber)
{
  assert(param.FrameStep > 0 && mediaFrameNumber >= param.SourceStartFrameNumber);
  return mediaFrameNumber - param.SourceStartFrameNumber;
}
#pragma endregion

#pragma region Epilog
#endif // __MERCALLICLI_VIDEOSTABILIZATION_H

#pragma endregion