#pragma region Prolog
#ifndef __INC_MERCALLI_INCLUDE_H
#define __INC_MERCALLI_INCLUDE_H
/*******************************************************************
 $CRT 26 Apr 2007 : hb

 $AUT Holger Burkarth
 $DAT >>mercalli.h<< 27 Mai 2025  09:14:53 - (c) proDAD
*******************************************************************/
#pragma endregion

#pragma region +++ Mercalli SDK +++
/* Mercalli SDK
 * This Software Development Kit (SDK) is provided under license from
 * proDAD GmbH and may only be used in accordance with the terms of that license.
 *
 * proDAD GmbH
 * Gauertstr. 2
 * D-78194 Immendingen, Germany
 * Phone: +49 (0)7462 9459 0 | Fax: +49 (0)7462 9459 79
 * For SDK technical support, please contact: sdk@prodad.com
 *
 * Features:
 * - Removal of CMOS distortions (Unjello)
 * - Camera path smoothing (Stabilization)
 * - Support for fisheye cameras
 */
#pragma endregion

#pragma region History Log / Version Info
/*
 * 0x0000FFFF - Sub-Version
 * 0xFFFF0000 - Main-Version
 */
#define MERCALLI_INTERFACE_VERSION 0x0006000b

/*
 * 2022-01-09: 6.11
 *    - Update for Mercalli 6.0
 * 2017-06-07: 4.10
 *   - Added product activation support
 * 2015-12-10: 4.9
 *   - Added support for additional camera intrinsics
 * 2015-05-02: 4.8
 *   - Added DirectX support
 *   - Added UnjelloRenderSystem_Enum
 *   - Added UJMthTyp_PTek_OF_Fast
 * 2015-01-15: 4.7
 *   - Reduced memory consumption
 *   - Added support for interlaced video processing
 * 2014-11-10: 4.6
 *   - Improved robustness of motion detection for fisheye cameras
 *   - Improved rendering performance
 *   - Changed SDK samples to YUV format
 *   - Added support for additional camera intrinsics
 * 2014-09-12: 4.5
 *   - Added new profiles for hyper-lapse
 *   - Added function: MercalliFrameNumberFromMediaFrame()
 *   - Fixed bug in UJMthTyp_PTek_FP
 * 2014-09-05: 4.4
 *   - Improved correction effect of UJMthTyp_PTek_FP
 * 2014-09-01: 4.3
 *   - Added license service
 * 2014-08-30: 4.2
 *   - Added CameraIntrinsic_Enum
 * 2014-08-21: 4.1
 *   - Initial release
 */

//END History Log / Version Info
#pragma endregion

#pragma region Includes
#include <proDADTypes.h>
#include <proDADGuid.h>
#include <assert.h>
#include <math.h>

#if defined(MERCALLISDK_BUILD_STATICLIB)
  #define MERCALLISDK_API(type, name) pMETHODE_LINKLIB_API(type, name)
#elif defined(MERCALLISDK_LINK_STATICLIB)
  #define MERCALLISDK_API(type, name) pMETHODE_LINKLIB_API(type, name)
#elif defined(MERCALLISDK_BUILD_DYNLIB)
  #define MERCALLISDK_API(type, name) pMETHODE_EXPORT_API(type, name)
#elif defined(MERCALLISDK_LINK_DYNLIB)
  #define MERCALLISDK_API(type, name) pMETHODE_IMPORT_API(type, name)
#else
  #error Unknown link-option, please define first
#endif

#ifndef __cplusplus
  #error Mercalli SDK must be compiled as C++
#endif

#ifdef _MSC_VER
#pragma managed(push, off)

#pragma warning ( push )
/* Warnung	C26495	Die Variable ist nicht initialisiert.
 * Eine Membervariable muss immer initialisiert werden (type.6).
 */
#pragma warning( disable : 26495 )

/* Warnung C26812: ' enum class ' vor ' ENUM ' bevorzugen (Enum. 3) */
#pragma warning( disable : 26812 )
#endif

//END Includes
#pragma endregion

#pragma region extern C begin

#ifdef __cplusplus
extern "C" {
#endif

#pragma endregion

#pragma region Declarations
struct T_GlobalMotionPath;
struct T_Unjello;
struct T_SettingsB;
struct T_GmpScanImageParam;
struct Bitmap_Type;
struct T_UnjelloPushFrameParam;
struct T_UnjelloFrameArguments;
struct T_UnjelloRenderFrameParam;

#pragma endregion

#pragma region Enums

#pragma region enum GfxFormat_Enum
typedef enum GfxFormat_Enum
{
  FMT_Unknown = 0,
  FMT_BGRA8,     // Order: B,G,R,A -- 8-bit per Channel;  32-bit Pixel ;  1-Plane
  FMT_BGR8,      // Order: B,G,R   -- 8-bit per Channel;  24-bit Pixel ;  1-Plane
  FMT_ARGB8,     // Order: A,R,G,B -- AfterEffects, FCP                ;  1-Plane
  FMT_UYVY8,     // Y at Idx=1,3 - e.g.: Liquid, Studio                ;  1-Plane
  FMT_YUYV8,     // Y at Idx=0,2 - e.g.: EDIUS                         ;  1-Plane
  FMT_VUYA8,     // Premiere                                           ;  1-Plane
  FMT_AYUV8,     // Order: A,Y,U,V -- FCP                              ;  1-Plane
  FMT_BGR16,     // Order: B,G,R   -- 16bit per Channel;   48-bit Pixel;  1-Plane
  FMT_BGRA16,    // Order: B,G,R,A -- 16bit per Channel;   64-bit Pixel;  1-Plane
  FMT_BGRA32f,   // Order: B,G,R,A -- 32bit float values; 128-bit Pixel;  1-Plane
  FMT_ARGB16,    // Order: A,R,G,B -- 16bit per Channel;   64-bit Pixel;  1-Plane
  FMT_ARGB32f,   // Order: A,R,G,B -- 32bit float values; 128-bit Pixel;  1-Plane
  FMT_AYUV16,    // Order: A,Y,U,V -- 16bit per Channel;   64-bit Pixel;  1-Plane
  FMT_AYUV32f,   // Order: A,Y,U,V -- 32bit float values; 128-bit Pixel;  1-Plane
  FMT_VUYA16,    // Order: V,U,Y,A -- 16bit per Channel;   64-bit Pixel;  1-Plane
  FMT_VUYA32f,   // Order: V,U,Y,A -- 32bit float values; 128-bit Pixel;  1-Plane
  FMT_Gray8,     // Unsigned 8-Bit                                     ;  1-Plane
  FMT_Gray16,    // Unsigned 16-Bit                                    ;  1-Plane
  FMT_Gray32f,   // 32-bit float                                       ;  1-Plane
  FMT_VUY8,      // Order:V,U,Y    --  8bit per Channel;   24-bit Pixel;  1-Plane
  FMT_VUY16,     // Order:V,U,Y    -- 16bit per Channel;   48-bit Pixel;  1-Plane
  FMT_VUY32f,    // Order:V,U,Y    -- 32bit float values;  96-bit Pixel;  1-Plane
  FMT_YUV420P,   // P0: 1:1 Y-8bit, P1: 2:2 U-8bit, PB2: 2:2 V-8bit    ;  3-Planes
  FMT_YUYV10,    // Y at Idx=0,2 - 16bit pro Value e.g.: EDIUS         ;  1-Plane
  FMT_RGBA32f,   // Order: B,G,R,A -- 32bit float values; 128-bit Pixel;  1-Plane
  FMT_RGBA8,     // Order: R,G,B,A -- 8-bit per Channel;  32-bit Pixel ;  1-Plane
  FMT_Gray10,    // Unsigned 10-Bit                                    ;  1-Plane
  FMT_Gray12,    // Unsigned 12-Bit                                    ;  1-Plane

} format_type;
#pragma endregion

#pragma region enum FieldFormat_Enum
typedef enum FieldFormat_Enum
{
  FLD_Full = 0,
  FLD_Even = 1,
  FLD_Odd  = 2,

  /* Frame with both fields (interlaced fields)
   * - Lines 0, 2, 4, ... are the even field
   * - Lines 1, 3, 5, ... are the odd field
   *
   * Field Dominance:
   * - FLD_Weaved | FLD_Even: Even field is the first
   * - FLD_Weaved | FLD_Odd: Odd field is the first
   */
  FLD_Weaved = 4,

  FLD_Weaved_Even = FLD_Weaved | FLD_Even, // even first
  FLD_Weaved_Odd  = FLD_Weaved | FLD_Odd,  // odd first

} field_type;
#pragma endregion

#pragma region enum BorderParam_Enum
enum BorderParam_Enum
{
  BorderParam_Expand,       //!< obsolete
  BorderParam_Crop,         //!< obsolete
  BorderParam_DynCrop,
  BorderParam_Raw,          //!< obsolete
  BorderParam_StaticExpand, //!< obsolete
  BorderParam_DynBump,      //!< obsolete
  BorderParam_Suppress,     //!< Best
  BorderParam_Forensic,     //!<
};
#pragma endregion

#pragma region enum CompensationKind_Enum
enum ComplensationKind_Enum
{
  CompensationKind_Unk = 0x00,
  CompensationKind_TX  = 0x01,
  CompensationKind_TY  = 0x02,
  CompensationKind_TZ  = 0x04,
  CompensationKind_RZ  = 0x08,

  CompensationKind_Move        = CompensationKind_TX | CompensationKind_TY,
  CompensationKind_MoveRot     = CompensationKind_TX | CompensationKind_TY | CompensationKind_RZ,
  CompensationKind_MoveRotZoom = CompensationKind_TX | CompensationKind_TY | CompensationKind_TZ | CompensationKind_RZ,
};
#pragma endregion

#pragma region enum ScanAlgo_Enum
/* Internal use only */
enum ScanAlgo_Enum
{
  ScanAlgo_Universal2D = 1, // alias Turbo
  ScanAlgo_Universal3D,
  ScanAlgo_AI_2D,
  ScanAlgo_AI_3D,
  ScanAlgo_Exp1,
  ScanAlgo_Exp2,
};
#pragma endregion

#pragma region enum ScanFlags_Enum
enum ScanFlags_Enum
{
  ScanFlags_Adjust         = 0x01, /* internal */
  //ScanFlags_Edge           = 0x02, /* internal */
  //ScanFlags_ExtremeMoves   = 0x04, /* internal */
  //ScanFlags_Fein           = 0x08, /* internal */
  ScanFlags_GlobFrameRS    = 0x10, /* enable global frame rolling-shutter compensation */
  ScanFlags_SmartMotions   = 0x40, /* internal (Glide) */
  ScanFlags_TryVariation   = 0x80, /* Find the best Variationen. Only supported for profile (42). */
};
#pragma endregion

#pragma region enum ViewFlags_Enum
enum ViewFlags_Enum
{
  ViewFlags_FillBorder      = 0x0100, /* enable border fill-in */
  ViewFlags_DynamicCamera   = 0x1000, /* enable dyn. camera path. Keep original camera path. */
  ViewFlags_XRotationAxis   = 0x2000, /* enable X-Axis compensation */
  ViewFlags_YRotationAxis   = 0x4000, /* enable Y-Axis compensation */
};
#pragma endregion

#pragma region enum FinishPathFlags_Enum
enum FinishPathFlags_Enum
{
  FinPathFlags_None = 0,

  /*! Do not delete scan data to enable incremental scanning or rendering
   */
  FinPathFlags_KeepScanDatas = 0x04,

  /*! Scan can be continued
   */
  FinPathFlags_Continuable = 0x08,

  /*! 12.02.2013: Try different variations.
   * The ScanFlags_TryVariation bit must be set in Settings.ScanFlags.
   */
  FinPathFlags_SupportVariation = 0x10,

};
#pragma endregion

#pragma region enum NewGlobalMotionPathFlags_Enum
enum NewGlobalMotionPathFlags_Enum
{
  /*! Analysis for interlace images with weaved fields (see FLD_EvenOdd)
   * Each frame includes two fields!
   * This is the only supported interlaced method.
   */
  NewGMPFlags_Fields = 0x01,

  /*! (Internal use only)
   * Retain motion data during the analysis process.
   */
  NewGMPFlags_KeepMotionDatas = 0x02,
};
#pragma endregion

#pragma region enum CameraProjectionType_Enum
enum CameraProjectionType_Enum
{
  CamProjTyp_Unknown,
  CamProjTyp_Polynomial,
  CamProjTyp_Orthographic,
};
#pragma endregion

#pragma region enum CameraIntrinsic_Enum
enum CameraIntrinsic_Enum
{
  // Non-Fisheye Cam
  CamItic_Zero,

  // GoPro96
  CamItic_GoProHero96_Orthographic,

  // GoPro Hero3+
  CamItic_GoProHero3pWide_Polynomial,
  CamItic_GoProHero3pWide_Orthographic,
  CamItic_GoProHero3pMedium_Orthographic,
  CamItic_GoProHero3pNarrow_Orthographic,

  // GoPro Hero3
  CamItic_GoProHero3Medium1080_Orthographic,
  CamItic_GoProHero3Wide1080_Orthographic,
  CamItic_GoProHero3ic1440_Orthographic,
  CamItic_GoProHero3ic1080_Orthographic,
  CamItic_GoProHero3ic960_Orthographic,
  CamItic_GoProHero3ic720_Orthographic,
  CamItic_GoProHero3ic4KCin_Orthographic,
  CamItic_GoProHero3ic4K_Orthographic,
  CamItic_GoProHero3ic27KCin_Orthographic,
  CamItic_GoProHero3ic27K_Orthographic,

  // GoPro Hero2
  CamItic_GoProHero2nc1080w_Orthographic,
  CamItic_GoProHero2nc960w_Orthographic,
  CamItic_GoProHero2nc720_Orthographic,

  // GoPro Hero4
  CamItic_GoProHero4Wide_Orthographic,
  CamItic_GoProHero4Medium_Orthographic,
  CamItic_GoProHero4Narrow_Orthographic,
  CamItic_GoProHero4Session1080Medium_Orthographic,
  CamItic_GoProHero4Session1080Wide_Orthographic,
  CamItic_GoProHero4Session1440Wide_Orthographic,

  // GoPro Hero5
  CamItic_GoProHero5Wide_Orthographic,
  CamItic_GoProHero5Wide1440_Orthographic,
  CamItic_GoProHero5Medium_Orthographic,
  CamItic_GoProHero5Narrow_Orthographic,
  CamItic_GoProHero5SessionWide_Orthographic,
  CamItic_GoProHero5SessionWide1440_Orthographic,
  CamItic_GoProHero5SessionMedium_Orthographic,
  CamItic_GoProHero5SessionNarrow_Orthographic,

  // GoPro Hero6
  CamItic_GoProHero6WideUnstabilized_Orthographic,
  CamItic_GoProHero6WideStabilized_Orthographic,

  // GoPro Hero (Extras)
  CamItic_GoProHeroPlus_Orthographic,
  CamItic_GoProHero4_44mmFlatLens_Orthographic,

  // DJI Phantom Vision
  CamItic_DJIPhantom2Vision_Orthographic,
  CamItic_DJIPhantom2VisionPlus_Orthographic,
  CamItic_DJIPhantom3_Orthographic,
  CamItic_DJIPhantom4_Orthographic,
  CamItic_DJIPhantom5_Orthographic,

  // Sony
  CamItic_SonyHDRAS10nc1080_Orthographic,
  CamItic_SonyHDRAS10ic1080_Orthographic,
  CamItic_SonyAS100VSteadyOff_Orthographic,
  CamItic_SonyAS100VSteadyOn_Orthographic,
  CamItic_SonyHDRAZ1SteadyOff_Orthographic,
  CamItic_SonyHDRAZ1SteadyOn_Orthographic,
  CamItic_SonyFDRX1000VSteadyOff_Orthographic,
  CamItic_SonyFDRX1000VSteadyOn_Orthographic,
  CamItic_SonyFDRAX53_Orthographic,
  CamItic_SonyFDRX3000R_Orthographic,
  CamItic_SonyFDRX3000Wide_Orthographic,
  CamItic_SonyFDRX3000Medium_Orthographic,
  CamItic_SonyFDRX3000Narrow_Orthographic,

  // Panasonic
  CamItic_PanasonicHXA500_Orthographic,

  // Contour
  CamItic_ContourHDMod1300_Orthographic,
  CamItic_Contour2Plus720_Orthographic,
  CamItic_Contour2Plus1080_Orthographic,
  CamItic_ContourROMA3170_Orthographic,
  CamItic_ContourROMA3125_Orthographic,

  // Rollei
  CamItic_RolleiBulletHD_Orthographic,
  CamItic_RolleiBulletS5_Orthographic,
  CamItic_RolleiS30_Orthographic,
  CamItic_RolleiS40_Orthographic,
  CamItic_RolleiS7w_Orthographic,
  CamItic_RolleiS7m_Orthographic,
  CamItic_RolleiS7n_Orthographic,
  CamItic_RolleiS7s_Orthographic,
  CamItic_RolleiS7_4k_Orthographic,

  // Canon
  CamItic_CanonLEGRIAminiXwide_Orthographic,
  CamItic_CanonEosC100_Orthographic,
  CamItic_CanonG15_Orthographic,

  // Drift
  CamItic_DriftInnovationsHD170_Orthographic,
  CamItic_DriftGhostS090_Orthographic,
  CamItic_DriftGhostS127_Orthographic,
  CamItic_DriftGhostS160_Orthographic,
  CamItic_DriftStealth_Orthographic,

  // SOMIKON
  CamItic_SomikonDV800Pearl1080_Orthographic,
  CamItic_SomikonDV800Pearl720_Orthographic,
  CamItic_SomikonDV78night720_Orthographic,

  // VIVITAR
  CamItic_VivitarDVR785HD720_Orthographic,

  // HP
  CamItic_HPac200ic1080_Orthographic,

  // Ion
  CamItic_IonAirPro720_Orthographic,
  CamItic_IonAirPro1080_Orthographic,

  // GARMIN
  CamItic_GarminVIRB1080_Orthographic,

  // Toshiba
  CamItic_ToshibaXSports1080_Orthographic,
  CamItic_BraunMaster_AEES51medium_Orthographic,
  CamItic_BraunMaster_AEES5narrow_Orthographic,
  CamItic_BraunMaster_AEES5little_Orthographic,

  // BraunMaster / AEE
  CamItic_BraunMaster_AEES51wide_Orthographic,

  // ActionPro
  CamItic_ActionProX7wide_Orthographic,
  CamItic_ActionProX7medium_Orthographic,
  CamItic_ActionProX7narrow_Orthographic,

  // GoXtreme Wifi Control
  CamItic_GoXtremeWifiControl_Orthographic,

  // MOBIUS
  CamItic_MobiusAWide_Orthographic,
  CamItic_MobiusANarrow1080_Orthographic,
  CamItic_MobiusBWide_Orthographic,
  CamItic_MobiusBNarrow1080_Orthographic,
  CamItic_MobiusCWide_Orthographic,
  CamItic_MobiusCNarrow1080_Orthographic,

  // FOXEER Legend 1
  CamItic_FoxeerLegend1Narrow_Orthographic,
  CamItic_FoxeerLegend1Medium_Orthographic,
  CamItic_FoxeerLegend1Wide_Orthographic,

  // SJcam
  CamItic_SJcam5000xWide_Orthographic,
  CamItic_SJcam5000Plus_Orthographic,
  CamItic_SJcamM10_Orthographic,

  // RunCam
  CamItic_RunCam2_Orthographic,

  // VEHO
  CamItic_VehoMUVIKSeries80_Orthographic,
  CamItic_VehoMUVIKSeries100_Orthographic,
  CamItic_VehoMUVIKSeries120_Orthographic,
  CamItic_VehoMUVIKSeries140_Orthographic,

  // EKEN
  CamItic_EkenH8R_Orthographic,

  // OLLIVAN
  CamItic_OllivanGIT2_120_Orthographic,
  CamItic_OllivanGIT2_170_Orthographic,

  // Misc
  CamItic_YI4K_Orthographic,

};
#pragma endregion

#pragma region enum CameraIntrinsicLevel_Enum
enum CameraIntrinsicLevel_Enum
{
  CamIticLev_Source       = 0,
  CamIticLev_Target       = 1,
  CamIticLev_LastAnalyzed = 2,

};
#pragma endregion

#pragma region enum LibValueType_Enum
enum LibValueType_Enum
{
  LIBVTyp_Unknown,
  LIBVTyp_AddBinLicense,
  LIBVTyp_AddTextLicense,
  LIBVTyp_OpenProductActivationSession,
  LIBVTyp_CloseProductActivationSession,
  LIBVTyp_GenerateProductActivationCaller,
  LIBVTyp_SetProductActivationCaller,
  LIBVTyp_GetProductActivationCaller,
  LIBVTyp_SetProductActivationBag,
  LIBVTyp_GetProductActivationBag,
  LIBVTyp_GetProductActivationURL,
  LIBVTyp_ActivateProduct,
  LIBVTyp_BeginActivateProduct,
  LIBVTyp_FinishActivateProduct,
  LIBVTyp_GetActivateProductStatus,
};
#pragma endregion

#pragma region enum GmpValueType_Enum
enum GmpValueType_Enum
{
  GMPVTyp_Unknown,
  GMPVTyp_CurrentAnalysisFrameCount,
  GMPVTyp_AnalysisFrameTranslation,
  GMPVTyp_AnalysisFrameRotation,
  GMPVTyp_CameraIntrinsic,
  GMPVTyp_FadeInFrameCount,
  GMPVTyp_FadeOutFrameCount,
  GMPVTyp_CurrentAnalysisProgress,

};
#pragma endregion

#pragma region enum MRemapMode_Enum
enum MRemapMode_Enum
{
  MREMPMD_Unknown,
  MREMPMD_Nearest,
  MREMPMD_Linear,
  MREMPMD_Cubic,
};
#pragma endregion

#pragma region enum SplitscreenType_Enum
enum SplitscreenType_Enum
{
  SPLTSCTY_OnlySource = -1,
  SPLTSCTY_OnlyTarget = 0,
  SPLTSCTY_LeftToRightSplit,
  SPLTSCTY_TopToBottomSplit,
};
#pragma endregion

#pragma region enum UnjelloBorderFillMode_Enum
enum UnjelloBorderFillMode_Enum
{
  UJBFILMD_Default = -1,
  UJBFILMD_None    =  0,
  UJBFILMD_Inpaint, /* Best */
  UJBFILMD_Reflect,
  UJBFILMD_Replicate,
};
#pragma endregion

#pragma region enum UnjelloMethodType_Enum
enum UnjelloMethodType_Enum
{
  UJMthTyp_Unknown,
  UJMthTyp_PTek_OF,
  UJMthTyp_PTek_FP,
  UJMthTyp_PTek_OF_Fast,
  UJMthTyp_TTek_OF,
};
#pragma endregion

#pragma region enum UnjelloTRemoveWaves_Enum
enum UnjelloTRemoveWaves_Enum
{
  UJRMWav_Mean,      /* Remove mean waves */
  UJRMWav_Long,      /* Remove long waves */
  UJRMWav_UltraLong, /* Remove extrem long waves */
};
#pragma endregion

#pragma region enum UnjelloTRemoveVibration_Enum
enum UnjelloTRemoveVibration_Enum
{
  UJRMVibr_Low,       /* Remove low vibration frequency */
  UJRMVibr_Mean,      /* Remove mean vibration frequency */
  UJRMVibr_High,      /* Remove high vibration frequency */
  UJRMVibr_UltraHigh, /* Remove ultra high vibration frequency */
};
#pragma endregion

#pragma region enum UnjelloRenderSystem_Enum
enum UnjelloRenderSystem_Enum
{
  UJRSYS_Unknown,
  UJRSYS_Maximum,       /* (Default) Use all possible TEK like: detached multithreading pre-load */
  UJRSYS_SyncPreload,   /* multithreading pre-load sync. for each render-job */

  /* No pre-load; no forward-frame needed.
   * Works by one frame shifted. And works only at forward direction.
   */
  UJRSYS_Latency1, // 1 Frame
  UJRSYS_Latency2, // 2 Frames
  UJRSYS_Latency3, // 3 Frames

};
#pragma endregion

#pragma region enum UnjelloValueType_Enum
enum UnjelloValueType_Enum
{
  UJVTyp_Unknown,
  UJVTyp_CustomData,
  UJVTyp_UnjelloFrameCallback,
  UJVTyp_ShutterSpeed,
  UJVTyp_EnableAutoShutterSpeed,
  UJVTyp_AverageShutterSpeed,
  UJVTyp_FrameNumberRange,
  UJVTyp_Method,
  UJVTyp_EstimateShutterSpeedRequired,
  UJVTyp_BorderFillMode,
  UJVTyp_RemapMode,
  UJVTyp_SplitscreenView,
  UJVTyp_BrandingView,
  UJVTyp_MethodAvailable,
  UJVTyp_FlushCount,
  UJVTyp_GpuDeviceCount,
  UJVTyp_EnableFrameCallback,
  UJVTyp_LatencyRenderEnabled,
  UJVTyp_LatencyRenderPossible,
  UJVTyp_LatencyFrameCount,
  UJVTyp_RenderSystem,
};
#pragma endregion


//END enums
#pragma endregion

#pragma region GUIDs
// {8F019C1A-A46E-4de8-9896-DE958EF767CC}
DEFINE_GUID(MercalliMagix_DataStream, 0x8f019c1a, 0xa46e, 0x4de8, 0x98, 0x96, 0xde, 0x95, 0x8e, 0xf7, 0x67, 0xcc);

// {AB0DE48A-6A5B-4f7d-9D64-D60F71112E50}
DEFINE_GUID(MercalliStreamKey_SmoothFrameMatrix, 0xab0de48a, 0x6a5b, 0x4f7d, 0x9d, 0x64, 0xd6, 0xf, 0x71, 0x11, 0x2e, 0x50);

// {34D230A8-6A71-4b10-906F-E1BD78A8CDB9}
DEFINE_GUID(MercalliStreamKey_RealFrameMatrix, 0x34d230a8, 0x6a71, 0x4b10, 0x90, 0x6f, 0xe1, 0xbd, 0x78, 0xa8, 0xcd, 0xb9);

// {57A2D22B-A383-43ec-A9D0-3745E92ACB87}
DEFINE_GUID(MercalliStreamKey_MediaInfo, 0x57a2d22b, 0xa383, 0x43ec, 0xa9, 0xd0, 0x37, 0x45, 0xe9, 0x2a, 0xcb, 0x87);

// {AB969282-18D7-407a-A3C6-16115758300D}
DEFINE_GUID(MercalliStreamKey_SettingsB, 0xab969282, 0x18d7, 0x407a, 0xa3, 0xc6, 0x16, 0x11, 0x57, 0x58, 0x30, 0xd);

// {517D5DEB-42D1-44c4-912D-2BC8CBE66E98}
DEFINE_GUID(MercalliStreamKey_CameraIntrinsic, 0x517d5deb, 0x42d1, 0x44c4, 0x91, 0x2d, 0x2b, 0xc8, 0xcb, 0xe6, 0x6e, 0x98);

#pragma endregion

#pragma region Structs

#pragma region struct Bitmap_Type
struct Bitmap_Type
{
  SINT32  Width{};
  SINT32  Height{};
  FLOAT32 PAR{};     // 0.0 -- ignore, 1.067=PAL, 1.3333=HDV, etc.
  UINT32  Format{};  // (enum format_type)

  struct Plane_Type
  {
    SINT_PTR Pitch;
    UINT8*   Data;
  } Plane[4]{};


#ifdef HAVE_OPENCV_UMAT
  cv::UMat* pUMat{};

#else
  void* Pad0{};
#endif
};
#pragma endregion

#pragma region struct T_SettingsB
struct T_SettingsB
{
  SINT32 ViewScale;    /* 0=none, 65536=maximum */
  SINT32 CompKnd;      /* (enum CompensationKind_Enum) */
  SINT32 ScanFlags;    /* (enum ScanFlags_Enum) */
  SINT32 ScanAlgo;     /* (enum ScanAlgo_Enum) */
  SINT32 TransFac;     /* 0=min. 65536=static */
  SINT32 BorderParam;  /* (enum BorderParam_Enum) */
  SINT32 ViewFlags;    /* (enum ViewFlags_Enum) */
  SINT32 ZoomFac;      /* 0=min. 65536=static */
  SINT32 ZRotationFac; /* 0=min. 65536=static */
  SINT32 XRotationFac; /* 0=min. 65536=static */
  SINT32 YRotationFac; /* 0=min. 65536=static */
  SINT32 DynScaleFac;  /* 0=static 65536=fast adjust */
};
#pragma endregion

#pragma region struct T_CameraIntrinsic
struct T_CameraIntrinsic
{
  SINT32   Width, Height; // Pixel-Resolution
  FLOAT32  Fx, Fy;        // Focal Length
  FLOAT32  Px, Py;        // Principal Point
  UINT32   Projection;    // (CameraProjectionType_Enum)
  FLOAT64  K[4];          // Distortion coefficient
  FLOAT64  P[2];          // Distortion coefficient
};
#pragma endregion

#pragma region struct T_MercalliRenderExtraParam
struct T_MercalliRenderExtraParam
{
  // (enum field_type)
  UINT32 Field;

  // extra Z-Rotation (rad)
  FLOAT32 ZRotationRad;

  // extra Zoom
  FLOAT32 Zoom;

  // (MRemapMode_Enum)
  SINT32 RemapMode;

};
#pragma endregion

#pragma region struct T_MercalliRenderFrameParam
struct T_MercalliRenderFrameParam
{
  T_MercalliRenderExtraParam Extra;

  /* Source, Target
   *
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   * !! Very Important: Top-Line must First-Line !!
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  Bitmap_Type Src, Tar;

};
#pragma endregion

#pragma region struct T_GmpScanImageParam
struct T_GmpScanImageParam
{
  // (enum field_type)
  SINT32  Field;

  /* Image
   *
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   * !! Very Important: Top-Line must First-Line !!
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  Bitmap_Type Image;

  // Frame Rate
  FLOAT32 Rate; // 0.0 -- ignore, 25.0=PAL, 29.97=PAL, etc.

  T_SettingsB Settings;
};
#pragma endregion

#pragma region +DataStream Structs

#pragma region struct T_MercalliStreamFrameMatrixKey
struct T_MercalliStreamFrameMatrix
{
  FLOAT32 R[3][3], T[2];
  FLOAT32 ZoomIn;
};

/* MercalliStreamKey_SmoothFrameMatrix
 * MercalliStreamKey_RealFrameMatrix
 */
struct T_MercalliStreamFrameMatrixKey
{
  SINT64 Count; // Number of Items[]
  struct T_MercalliStreamFrameMatrix Items[1];
};
#pragma endregion

#pragma region struct T_MercalliStreamMediaInfo
/*MercalliStreamKey_MediaInfo */
struct T_MercalliStreamMediaInfo
{
  SINT32  MediaWidth;  // Media-Width
  SINT32  MediaHeight; // Media-Height
  SINT32  SWidth;
  SINT32  SHeight;
  SINT32  FieldOrder; // (field_type) from Scanning
  UINT32  Flags;      // (MotionCompArrayB_Flags)
  FLOAT32 PAR;        // PixelAspectRatio: 1.06 PAL
  FLOAT32 Rate;       // Media Frame-Rate: 25=PAL, 29.97=NTSC
  SINT32  FrameCount; // Number of stabilized Frames (or Fields by Interlace)
};
#pragma endregion

#pragma region struct T_MercalliStreamSettingsB
/* MercalliStreamKey_SettingsB */
struct T_MercalliStreamSettingsB
{
  /* Current used Settings/Profile */
  T_SettingsB CurrentSettings;
  SINT32      CurrentProfileID;

  /* Neue Settings, die später angewendet werden sollen.
   * Diese Struct dient nur zur Vormerkung der Settings.
   */
  T_SettingsB NewSettings;
  SINT32      NewProfileID;
};
#pragma endregion

#pragma region struct T_MercalliStreamCameraIntrinsic
/* MercalliStreamKey_CameraIntrinsic */
struct T_MercalliStreamCameraIntrinsic
{
  T_CameraIntrinsic SourceCameraIntrinsic;
  T_CameraIntrinsic TargetCameraIntrinsic;
};
#pragma endregion

#pragma region struct T_MercalliDataStream
struct T_MercalliDataStream
{
  GUID   Magic;      // Safety-Identifier: MercalliMagix_DataStream
  UINT64 StreamSize; // Size of the whole Stream in Bytes

  UINT32 ItemCount;  // Count of Items[]

  struct Item_Type
  {
    GUID   Key;     // MercalliStreamKey_XY or own Key
    UINT64 Offset;  // Byte-Offset to the Data-Block
    UINT64 Size;    // Byte-Size of the Data-Block
  } Items[1];       // max. Items[ItemCount]

  /* Public defined Keys:
   * T_MercalliStreamFrameMatrixKey as MercalliStreamKey_SmoothFrameMatrix
   * T_MercalliStreamFrameMatrixKey as MercalliStreamKey_RealFrameMatrix
   * T_MercalliStreamMediaInfo as MercalliStreamKey_MediaInfo
   * T_MercalliStreamSettingsB as MercalliStreamKey_SettingsB
   * T_MercalliStreamCameraIntrinsic as MercalliStreamKey_CameraIntrinsic
   *
   */

};
#pragma endregion

#pragma region struct T_MercalliStreamTag
struct T_MercalliStreamTag
{
        GUID    Key{};
        UINT64  Size{};
  const void*   Data{};
};
#pragma endregion


//END DataStream Structs
#pragma endregion

#pragma region struct T_UnjelloPushFrameParam
struct T_UnjelloPushFrameParam
{
  /* (FieldFormat_Enum) */
  UINT32 Field;

  /*
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   * !! Very Important: Top-Line must First-Line !!
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  Bitmap_Type Image;
};
#pragma endregion

#pragma region struct T_UnjelloFrameArguments
struct T_UnjelloFrameArguments
{
  SINT32 Width;
  SINT32 Height;
  UINT32 Format;  /* (enum format_type) */
  UINT32 Field;   /* (FieldFormat_Enum) */
  SINT64 FrameNumber;
};
#pragma endregion

#pragma region struct T_UnjelloRenderFrameParam
struct T_UnjelloRenderFrameParam
{
  /* (FieldFormat_Enum) */
  UINT32 Field;

  /* Target
   *
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   * !! Very Important: Top-Line must First-Line !!
   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  Bitmap_Type Image;

  /* Optional Source-Image.
   * @see UJVTyp_EnableFrameCallback
   * Only used if Src.Format != FMT_Unknown
   */
  Bitmap_Type Src;
};
#pragma endregion

#pragma region struct T_UnjelloTSettings
struct T_UnjelloTSettings
{
  SINT32 FrameWindowSize;    /* Range: 5 ... 25 */
  SINT32 SuperRowsPerFrame;  /* Range: 5 ... 79 */
  SINT32 Iterations;         /* Range: 1 ... 9000 */
  bool   UseFeatures;
};
#pragma endregion

#pragma region struct T_UnjelloTSmartSettings
struct T_UnjelloTSmartSettings
{
  SINT32 RemoveWaves;     /* enum UnjelloTRemoveWaves_Enum */
  SINT32 RemoveVibration; /* enum UnjelloTRemoveVibration_Enum */
  bool   Sensible;
};
#pragma endregion


//END structs
#pragma endregion

#pragma region Callbacks

/*
 */
typedef STDCALLBACKDECL(UnjelloPushFrameCallback)(
        T_Unjello*               pUJ,
        void*                    pPushFrameHandle,
        SINT64                   frameNumber,
  const T_UnjelloPushFrameParam* pPF);

/*
 */
typedef STDCALLBACKDECL(UnjelloFrameCallback)(
        T_Unjello*               pUJ,
  const T_UnjelloFrameArguments* pArgs,
        UnjelloPushFrameCallback pushFrame,
        void*                    pPushFrameHandle);

/*
 */
typedef STDCALLBACKDECL(UnjelloProgressCallback)(
  const T_Unjello* pUJ,
        SINT64     currentFrameNumber,
        SINT64     startFrameNumber,
        SINT64     endFrameNumber);


//END Callbacks
#pragma endregion

#pragma region extern C end
#ifdef __cplusplus
}
#endif
#pragma endregion

#pragma region Inlines

/*! Determines if a rescan is required
 */
__STATIC_INLINE
bool MustRescan(const T_SettingsB* _old, const T_SettingsB* _new)
{
  assert(_old != NULL && _new != NULL);
  SINT32 ScanFlagsMask = ~0;

  /* A change of Rolling-Shutter Options or TryVariation
   * needs no rescan
   */
  ScanFlagsMask &= ~(ScanFlags_GlobFrameRS | ScanFlags_TryVariation);

  return
      ((_old->ScanFlags ^ _new->ScanFlags) & ScanFlagsMask) != 0 // test ScanFlags
    || _old->ScanAlgo != _new->ScanAlgo                          // test ScanAlgo
  ;
}


/*! Determines if recalculation is required
 */
__STATIC_INLINE
bool MustSmoothCalc(const T_SettingsB* _old, const T_SettingsB* _new)
{
  assert(_old != NULL && _new != NULL);
  return
      _old->TransFac     != _new->TransFac
   || _old->ZRotationFac != _new->ZRotationFac
   || _old->XRotationFac != _new->XRotationFac
   || _old->YRotationFac != _new->YRotationFac
   || _old->ZoomFac      != _new->ZoomFac
   || _old->DynScaleFac  != _new->DynScaleFac
   || _old->CompKnd      != _new->CompKnd

   /* difference in: GlobFrameRS, ViewFlags_DynamicCamera, ViewFlags_XRotationAxis, ViewFlags_YRotationAxis */
   || ((_old->ScanFlags ^ _new->ScanFlags) & (ScanFlags_GlobFrameRS))!=0
   || ((_old->ViewFlags ^ _new->ViewFlags) & (ViewFlags_DynamicCamera|ViewFlags_XRotationAxis|ViewFlags_YRotationAxis))!=0

   /* difference in ViewScale and BorderParam_Suppress is used (old or new) */
   || (_old->ViewScale != _new->ViewScale && (_old->BorderParam == BorderParam_Suppress || _new->BorderParam == BorderParam_Suppress) )
   /* Enable/Disable BorderParam_Suppress */
   || (_old->BorderParam != _new->BorderParam && (_old->BorderParam == BorderParam_Suppress || _new->BorderParam == BorderParam_Suppress) )
  ;
}

__STATIC_INLINE
void InitCamera(T_CameraIntrinsic* pCam)
{
  memset(pCam, 0, sizeof(T_CameraIntrinsic));
  pCam->Projection = CamProjTyp_Polynomial;
  pCam->Width  = 1920;
  pCam->Height = 1080;
  pCam->Fx     = 800;
  pCam->Fy     = 800;
  pCam->Px     = (FLOAT32)(1920/2);
  pCam->Py     = (FLOAT32)(1080/2);
}

__STATIC_INLINE
bool IsCameraIntrinsicDisabledOrInvalid(const T_CameraIntrinsic* pCam)
{
  return
     pCam == NULL
  || pCam->Width  <= 0
  || pCam->Height <= 0
  || pCam->Fx < 1
  || pCam->Fy < 1
  || pCam->Projection == CamProjTyp_Unknown
    ;
}

__STATIC_INLINE
bool IsEqual(const T_CameraIntrinsic* a, const T_CameraIntrinsic* b)
{
  assert(a != NULL && b != NULL);
  return
     a->Width  == b->Width
  && a->Height == b->Height
  && a->Projection == b->Projection
  && fabs(a->Fx - b->Fx) < 1e-9f
  && fabs(a->Fy - b->Fy) < 1e-9f
  && fabs(a->Px - b->Px) < 1e-9f
  && fabs(a->Py - b->Py) < 1e-9f
  && fabs(a->K[0] - b->K[0]) < 1e-15
  && fabs(a->K[1] - b->K[1]) < 1e-15
  && fabs(a->K[2] - b->K[2]) < 1e-15
  && fabs(a->K[3] - b->K[3]) < 1e-15
  && fabs(a->P[0] - b->P[0]) < 1e-15
  && fabs(a->P[1] - b->P[1]) < 1e-15
  ;
}

/* Sample Intrinsic */
__STATIC_INLINE
void SetTestCamera_Polynomial_A(T_CameraIntrinsic* pCam)
{
  memset(pCam, 0, sizeof(T_CameraIntrinsic));
  pCam->Projection = CamProjTyp_Polynomial;
  pCam->Width  = 1920;
  pCam->Height = 1080;

  pCam->Fx     = 1144.1027f;
  pCam->Fy     = 1143.475f;

  pCam->Px     = 972.374f;
  pCam->Py     = 555.831f;

  pCam->K[0] = -0.333782967626;
  pCam->K[1] =  0.17538283755;
  pCam->K[2] = -0.0692790067313;
  pCam->K[3] =  0.0106135995217;
}

__STATIC_INLINE bool SettingsEquals(
  const T_SettingsB* a,
  const T_SettingsB* b,
  UINT32 scanFlagsMask = ~0)
{
  assert(a != NULL && b != NULL);
  return
       a->BorderParam  == b->BorderParam
    && a->CompKnd      == b->CompKnd
    && a->ScanAlgo     == b->ScanAlgo
    && a->TransFac     == b->TransFac
    && a->ViewFlags    == b->ViewFlags
    && a->ViewScale    == b->ViewScale
    && a->XRotationFac == b->XRotationFac
    && a->YRotationFac == b->YRotationFac
    && a->ZRotationFac == b->ZRotationFac
    && a->ZoomFac      == b->ZoomFac
    && a->DynScaleFac  == b->DynScaleFac
    && (a->ScanFlags & scanFlagsMask) == (b->ScanFlags & scanFlagsMask)
    ;
}

__STATIC_INLINE
bool IsUnjelloTSettingsDisabledOrInvalid(const T_UnjelloTSettings* pS)
{
  return
     pS == NULL
  || pS->FrameWindowSize < 3
  || pS->SuperRowsPerFrame < 3
  || pS->Iterations < 1
    ;
}

__STATIC_INLINE
bool IsEqual(const T_UnjelloTSettings* a, const T_UnjelloTSettings* b)
{
  assert(a != NULL && b != NULL);
  return
     a->FrameWindowSize   == b->FrameWindowSize
  && a->SuperRowsPerFrame == b->SuperRowsPerFrame
  && a->Iterations        == b->Iterations
  && a->UseFeatures       == b->UseFeatures
  ;
}



// Inlines
#pragma endregion

#pragma region Functions

#pragma region Internal Functions
void MercalliMedullaBind();
void MercalliMedullaBindUnjello();
void MercalliMedullaBindStabilizer();
void Cleanup_Mercalli();

#ifdef OS_WINDOWS
BOOL Init_Mercalli(HINSTANCE hInst = NULL);
#else
BOOL Init_Mercalli();
#endif

#pragma endregion

#pragma region Library-Version Functions
/*
 * 0x000000000000FFFF -- Patch
 * 0x00000000FFFF0000 -- MERCALLI_TECHNOLGOY_REVISION
 * 0x0000FFFF00000000 -- Sub-Version  - MERCALLI_INTERFACE_VERSION
 * 0xFFFF000000000000 -- Main-Version - MERCALLI_INTERFACE_VERSION
 */
MERCALLISDK_API(UINT64, MercalliVersion)();

/* Checks the Mercalli DLL */
__STATIC_INLINE bool MercalliUsable()
{
  return ((MercalliVersion() >> 32) & 0xffffffff) == MERCALLI_INTERFACE_VERSION;
}
#pragma endregion

#pragma region Library Set Functions

/* Sets a value
 * @param type       -- (LibValueType_Enum)
 * @param pArg       -- Input argument, depending on the type
 * @param argSize    -- Byte size of pArg
 * @param pOption    -- Additional options, depending on the type
 * @param optionSize -- Byte size of pOption
 * @return HRESULT
 */
MERCALLISDK_API(HRESULT, MercalliSetValue)(
  /* in  */       SINT32     type,
  /* in  */ const void*      pArg,
  /* in  */       SINT32     argSize,
  /* in  */ const void*      pOption,
  /* in  */       SINT32     optionSize);

/*
 * @param pBuffer --
 * @param length  -- byte size
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliAddLicense(
  /* in  */ const UINT8* pBuffer,
  /* in  */       SINT32 length)
{
  return MercalliSetValue
  (
    LIBVTyp_AddBinLicense,
    pBuffer, length, // Arguments
    NULL, 0          // Result
  );
}

__STATIC_INLINE HRESULT MercalliAddLicense(
  /* in  */ const char* pText)
{
  return MercalliSetValue
  (
    LIBVTyp_AddTextLicense,
    pText, 0, // Arguments
    NULL, 0   // Result
  );
}

/* Open a Session. Don't forget to close it.
 * @param pConfigText -- from proDAD
 */
__STATIC_INLINE HRESULT MercalliOpenProductActivationSession(
  /* in  */ const char* pConfigText)
{
  return MercalliSetValue
  (
    LIBVTyp_OpenProductActivationSession,
    pConfigText, 0, // Arguments
    NULL, 0         // Result
  );
}

/* Close a Session
 */
__STATIC_INLINE HRESULT MercalliCloseProductActivationSession()
{
  return MercalliSetValue
  (
    LIBVTyp_CloseProductActivationSession,
    NULL, 0, // Arguments
    NULL, 0  // Result
  );
}

/* Set Caller
 * @param pText -- from earlier generated via
 */
__STATIC_INLINE HRESULT MercalliSetProductActivationCaller(
  /* in  */ const char* pText)
{
  return MercalliSetValue
  (
    LIBVTyp_SetProductActivationCaller,
    pText, 0, // Arguments
    NULL, 0   // Result
  );
}

/* Set Bag
 * @param pText -- from earlier...
 */
__STATIC_INLINE HRESULT MercalliSetProductActivationBag(
  /* in  */ const char* pText)
{
  return MercalliSetValue
  (
    LIBVTyp_SetProductActivationBag,
    pText, 0, // Arguments
    NULL, 0   // Result
  );
}

/*
 *
 */
__STATIC_INLINE HRESULT MercalliGenerateProductActivationCaller()
{
  return MercalliSetValue
  (
    LIBVTyp_GenerateProductActivationCaller,
    NULL, 0, // Arguments
    NULL, 0  // Result
  );
}

/*
 * @notice requires OpenProductActivationSession(), SetProductActivationCaller(), SetProductActivationBag()
 */
__STATIC_INLINE HRESULT MercalliActivateProduct()
{
  return MercalliSetValue
  (
    LIBVTyp_ActivateProduct,
    NULL, 0, // Arguments
    NULL, 0  // Result
  );
}

/*
 * @param pText -- Activation code
 */
__STATIC_INLINE HRESULT MercalliBeginActivateProduct(
  /* in  */ const char* pText)
{
  return MercalliSetValue
  (
    LIBVTyp_BeginActivateProduct,
    pText, 0, // Arguments
    NULL, 0   // Result
  );
}

/*
 * @param pText -- Web response
 * @notice requires BeginActivateProduct()
 */
__STATIC_INLINE HRESULT MercalliFinishActivateProduct(
  /* in  */ const char* pText)
{
  return MercalliSetValue
  (
    LIBVTyp_FinishActivateProduct,
    pText, 0, // Arguments
    NULL, 0   // Result
  );
}




#pragma endregion

#pragma region Library Get Functions

/* Retrieves a value
 * @param type       -- (LibValueType_Enum)
 * @param pArg       -- Input argument, depending on the type
 * @param argSize    -- Byte size of pArg
 * @param pResult    -- Result, depending on the type
 * @param resultSize -- Byte size of pResult
 * @return HRESULT
 */
MERCALLISDK_API(HRESULT, MercalliGetValue)(
  /* in  */       SINT32 type,
  /* in  */ const void*  pArg,
  /* in  */       SINT32 argSize,
  /* out */       void*  pResult,
  /* in  */       SINT32 resultSize);

/* Get Caller String
 * @param pResult    -- Text buffer or NULL for getting the required size (via HRESULT)
 * @param resultSize -- Byte size of pResult
 * @notice A opened session and a generated Caller is needed.
 */
__STATIC_INLINE HRESULT MercalliGetProductActivationCaller(
  /* out */       char*  pResult,
  /* in  */       SINT32 resultSize)
{
  return MercalliGetValue
  (
    LIBVTyp_GetProductActivationCaller,
    NULL, 0,             // Arguments
    pResult, resultSize  // Result
  );
}

/* Get Bag String
 * @param pResult    -- Text buffer or NULL for getting the required size (via HRESULT)
 * @param resultSize -- Byte size of pResult
 */
__STATIC_INLINE HRESULT MercalliGetProductActivationBag(
  /* out */       char*  pResult,
  /* in  */       SINT32 resultSize)
{
  return MercalliGetValue
  (
    LIBVTyp_GetProductActivationBag,
    NULL, 0,             // Arguments
    pResult, resultSize  // Result
  );
}

/* Get Bag String
 * @param pResult    -- Text buffer or NULL for getting the required size (via HRESULT)
 * @param resultSize -- Byte size of pResult
 */
__STATIC_INLINE HRESULT MercalliGetProductActivationURL(
  /* out */       char*  pResult,
  /* in  */       SINT32 resultSize)
{
  return MercalliGetValue
  (
    LIBVTyp_GetProductActivationURL,
    NULL, 0,             // Arguments
    pResult, resultSize  // Result
  );
}

/* Get Status
 * @return
 * - S_OK:    activated
 * - S_FALSE: not activated
 * - FAILED:  any failure
 */
__STATIC_INLINE HRESULT MercalliGetActivateProductStatus()
{
  return MercalliGetValue
  (
    LIBVTyp_GetActivateProductStatus,
    NULL, 0, // Arguments
    NULL, 0  // Result
  );
}

#pragma endregion

#pragma region Gmp Functions
/* Deletes a GMP instance created by @ref MercalliCreateGmp().
 */
MERCALLISDK_API(HRESULT, MercalliDeleteGmp)(
  /* in  */ T_GlobalMotionPath* pGMP);

/* Creates a new, empty GMP instance.
 * @param numOfFrames -- Number of frames (do not double for interlaced videos).
 * @param flags       -- Configuration flags:
 *    - NewGlobalMotionPathFlags_Fields: Enables interlaced video support
 * @return The instance must be freed using @ref MercalliDeleteGmp().
 *         Returns NULL in case of failure.
 */
MERCALLISDK_API(HRESULT, MercalliCreateGmp)(
  /* out */ T_GlobalMotionPath** pGMP,
  /* in  */ SINT64               numOfFrames,
  /* in  */ UINT32               flags);


/* Apply Settings.
 * @param flags             -- (FinishPathFlags_Enum)
 * @param pAdjustedSettings -- Ignored by NULL.
 *                             Will used if FinPathFlags_SupportVariation is set and the Settings are able.
 * @return HResult
 */
MERCALLISDK_API(HRESULT, MercalliGmpApplySettings)(
  /* in  */       T_GlobalMotionPath* pGMP,
  /* in  */       SINT32        flags,
  /* in  */ const T_SettingsB*  pSettings,
  /* out */       T_SettingsB*  pAdjustedSettings
 );

MERCALLISDK_API(HRESULT, MercalliGmpAbortAnalysis)(
  /* in  */       T_GlobalMotionPath* pGMP);

/* Scans an image
 * @return HRESULT
 *
 * @note Supported formats:
 * - FMT_Gray8
 * - FMT_Gray10
 * - FMT_Gray12
 * - FMT_Gray16
 * - FMT_BGR8
 * - FMT_BGRA8
 * - FMT_BGRA16
 * - FMT_BGRA32f
 * - FMT_ARGB8
 * - FMT_ARGB16
 * - FMT_ARGB32f
 * - FMT_AYUV8
 * - FMT_AYUV16
 * - FMT_AYUV32f
 * - FMT_VUYA8
 * - FMT_VUYA16
 * - FMT_VUYA32f
 * - FMT_UYVY8
 * - FMT_YUYV8
 * - FMT_YUYV10
 * - FMT_YUV420P
 */
MERCALLISDK_API(HRESULT, MercalliGmpScanImage)(
  /* in  */       T_GlobalMotionPath*  pGMP,
  /* in  */       SINT64               frameNumber,
  /* in  */ const T_GmpScanImageParam* pParam);


/* Render Frame
 * @return HResult
 *
 * @list Supported Formats:
 * - FMT_Gray8
 * - FMT_Gray16
 * - FMT_Gray32f
 * - FMT_BGR8
 * - FMT_BGR16
 * - FMT_BGRA8
 * - FMT_BGRA16
 * - FMT_BGRA32f
 * - FMT_VUYA8
 * - FMT_VUYA16
 * - FMT_VUYA32f
 * - FMT_VUY8
 * - FMT_VUY16
 * - FMT_VUY32f
 * - FMT_ARGB8
 * - FMT_ARGB16
 * - FMT_ARGB32f
 * - FMT_AYUV8
 * - FMT_AYUV16
 * - FMT_AYUV32f
 * - FMT_VUY8
 * - FMT_VUY16
 * - FMT_VUY32f
 */
MERCALLISDK_API(HRESULT, MercalliStreamRenderFrame)(
  /* in  */ const T_MercalliDataStream*       pDataStream,
  /* in  */       SINT64                      frameNumber,
  /* in  */ const T_MercalliRenderFrameParam* pParam);


//END GMP Functions
#pragma endregion

#pragma region Gmp Get Functions

/* Retrieves a GMP value
 * @param pGMP       -- GMP instance
 * @param type       -- (GmpValueType_Enum)
 * @param pArg       -- Input argument, depending on the type
 * @param argSize    -- Byte size of pArg
 * @param pResult    -- Result, depending on the type
 * @param resultSize -- Byte size of pResult
 * @return HRESULT
 */
MERCALLISDK_API(HRESULT, MercalliGetGmpValue)(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* in  */       SINT32              type,
  /* in  */ const void*               pArg,
  /* in  */       SINT32              argSize,
  /* out */       void*               pResult,
  /* in  */       SINT32              resultSize);

/*! Retrieves the current finished frame number during analysis.
 * @note Only available during analysis.
 * @param pCount -- Frame count [0, 1, 2, ...]
 * @return HRESULT
 */
__STATIC_INLINE HRESULT MercalliGetCurrentAnalysisFrameCount(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* out */       SINT64*             pCount)
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_CurrentAnalysisFrameCount,
    NULL, 0,                // Arguments
    pCount, sizeof(*pCount) // Result
  );
}

/*! Get the current analysis progress.
 * @note: Only available at analysis time.
 * @param value[] -- result [current, total]
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliCurrentAnalysisProgress(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* out */       SINT64 (&pValue)[2])
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_CurrentAnalysisProgress,
    NULL, 0,                // Arguments
    pValue, sizeof(pValue)  // Result
  );
}


/* Get the frame translation vector [X,Y,Z].
 * @note: Only available at analysis time.
 * @param frameNumber -- [0,1,2 ... GetCurrentAnalysisFrameCount-1]
 * @param value[] -- result [~ Pixel-Distance]
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetAnalysisFrameTranslation(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* in  */       SINT64              frameNumber,
  /* out */       FLOAT32             (&pValue)[3])
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_AnalysisFrameTranslation,
    &frameNumber, sizeof(frameNumber), // Arguments
    pValue, sizeof(pValue)             // Result
  );
}

/* Get the frame axis rotation-angle vector [X,Y,Z] in rad.
 * @note: Only available at analysis time.
 * @param frameNumber -- [0,1,2 ... GetCurrentAnalysisFrameCount-1]
 * @param value[] -- result [~ Pixel-Distance]
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetAnalysisFrameRotation(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* in  */       SINT64              frameNumber,
  /* out */       FLOAT32             (&pValue)[3])
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_AnalysisFrameRotation,
    &frameNumber, sizeof(frameNumber), // Arguments
    pValue, sizeof(pValue)             // Result
  );
}

/* Get used Camera Intrinsic.
 * @param level -- (CameraIntrinsicLevel_Enum)
 *                 CamIticLev_Source, CamIticLev_Target, CamIticLev_LastAnalyzed
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetCameraIntrinsic(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* in  */       SINT32              level,
  /* out */       T_CameraIntrinsic*  pCam)
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_CameraIntrinsic,
    &level, sizeof(level),           // Arguments
    pCam, sizeof(T_CameraIntrinsic)  // Result
  );
}

/*!
 * @param pCount [0,1,2, ...]
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetFadeInFrameCount(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* out */       SINT64*             pCount)
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_FadeInFrameCount,
    NULL, 0,                // Arguments
    pCount, sizeof(*pCount) // Result
  );
}

/*!
 * @param pCount [0,1,2, ...]
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetFadeOutFrameCount(
  /* in  */ const T_GlobalMotionPath* pGMP,
  /* out */       SINT64*             pCount)
{
  return MercalliGetGmpValue
  (
    pGMP,
    GMPVTyp_FadeOutFrameCount,
    NULL, 0,                // Arguments
    pCount, sizeof(*pCount) // Result
  );
}




//END GMP Get Functions
#pragma endregion

#pragma region Gmp Set Functions

/* Set GMP value
 * @param pGMP       --
 * @param type       -- (GmpValueType_Enum)
 * @param pArg       -- Input argument, depending on type
 * @param argSize    -- byte size of pArg
 * @param pOption    -- additional options, depending on type
 * @param optionSize -- byte size of pOption
 * @return HResult
 */
MERCALLISDK_API(HRESULT, MercalliSetGmpValue)(
  /* in  */ T_GlobalMotionPath* pGMP,
  /* in  */       SINT32     type,
  /* in  */ const void*      pArg,
  /* in  */       SINT32     argSize,
  /* in  */ const void*      pOption,
  /* in  */       SINT32     optionSize);


/* Set Camera Intrinsic.
 * @param level -- (CameraIntrinsicLevel_Enum)
 *                 CamIticLev_Source, CamIticLev_Target
 * @param pCam -- Reset the Cam via NULL
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetCameraIntrinsic(
  /* in  */       T_GlobalMotionPath* pGMP,
  /* in  */       SINT32              level,
  /* in  */ const T_CameraIntrinsic*  pCam = NULL)
{
  return MercalliSetGmpValue
  (
    pGMP,
    GMPVTyp_CameraIntrinsic,
    &level, sizeof(level),           // Arguments
    pCam, sizeof(T_CameraIntrinsic)  // Option
  );
}

/*
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetFadeInFrameCount(
  /* in  */       T_GlobalMotionPath* pGMP,
  /* in  */       SINT64              count)
{
  return MercalliSetGmpValue
  (
    pGMP,
    GMPVTyp_FadeInFrameCount,
    &count, sizeof(count),    // Arguments
    NULL, 0                   // Option
  );
}

/*
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetFadeOutFrameCount(
  /* in  */       T_GlobalMotionPath* pGMP,
  /* in  */       SINT64              count)
{
  return MercalliSetGmpValue
  (
    pGMP,
    GMPVTyp_FadeOutFrameCount,
    &count, sizeof(count),    // Arguments
    NULL, 0                   // Option
  );
}



#pragma endregion

#pragma region Stabi-Profile Functions

/* Retrieves the number of static stabilization profiles
 */
MERCALLISDK_API(SINT32, MercalliGetProfileNums)();

/* Applies profile data to pSettings.
 * Refer to MercalliSAL for comparing profile differences.
 *
 * @param index
 *   - 42: Intelligent-Universal (Best, includes FinPathFlags_SupportVariation)
 */
MERCALLISDK_API(BOOL, MercalliApplyProfile)(
  /* in  */ SINT32       index,
  /* out */ T_SettingsB* pSettings);

/* Compare Settings with Index.
 */
MERCALLISDK_API(BOOL, MercalliIsSameProfile)(
  /* in  */       SINT32       index,
  /* in  */ const T_SettingsB* pSettings);

/* Find Index of Profile.
 * @return (-1) if not found
 */
MERCALLISDK_API(SINT32, MercalliFindProfile)(
  /* in  */ const T_SettingsB* pSettings);

#pragma endregion

#pragma region Static Camera Intrinsic Functions
/*
 * @warn Do not use the index as a permanent identifier.
 *
 * Use CameraIntrinsic_Enum or ID (GUID) instead.
 */

/* Get Number of static Intrinsics
 * @return Numbers of Intrinsics
 */
MERCALLISDK_API(SINT32, MercalliGetStaticCameraIntrinsicNums)();

/* Find Index
 * @return Intrinsic-Index or (-1) if not found
 */
MERCALLISDK_API(SINT32, MercalliFindStaticCameraIntrinsicEnum)(
  /* in  */ CameraIntrinsic_Enum ci);
MERCALLISDK_API(SINT32, MercalliFindStaticCameraIntrinsicGuid)(
  /* in  */ const GUID& id);

/* Get Intrinsic
 * @param index -- Index of Range [0 ... GetStaticCameraIntrinsicNums-1]
 *                 e.g.: from FindStaticCameraIntrinsicEnum or FindStaticCameraIntrinsicGuid
 * @return 0 if succ.
 */
MERCALLISDK_API(HRESULT, MercalliGetStaticCameraIntrinsic)(
  /* in  */ SINT32             index,
  /* out */ T_CameraIntrinsic* pCam);

/* Get Intrinsic ID
 * @param index -- Index of Range [0 ... GetStaticCameraIntrinsicNums-1]
 *                 e.g.: from FindStaticCameraIntrinsicEnum or FindStaticCameraIntrinsicGuid
 * @return 0 if succ.
 */
MERCALLISDK_API(HRESULT, MercalliGetStaticCameraIntrinsicID)(
  /* in  */ SINT32 index,
  /* out */ GUID*  pID);

/* Get Intrinsic Name
 * @param index -- Index of Range [0 ... GetStaticCameraIntrinsicNums-1]
 *                 e.g.: from FindStaticCameraIntrinsicEnum or FindStaticCameraIntrinsicGuid
 * @param bufSize -- Byte-Size of pBuf
 * @return 0 if succ.
 */
MERCALLISDK_API(HRESULT, MercalliGetStaticCameraIntrinsicName)(
  /* in  */ SINT32   index,
  /* out */ UNICHAR* pBuf,
  /* in  */ SINT32   bufSize);

#pragma endregion

#pragma region DataStream Functions
/* Retrieves the T_MercalliStreamTag::Size value of the tags.
 * @param pGMP        -- GMP instance
 * @param pSourceTags -- Key-value pairs must be valid
 * @param pTargetTags -- Key, Size, and Data will be set (Data is usually NULL)
 * @param count       -- Number of elements in pSourceTags[] and pTargetTags[]
 * @return HRESULT
 *
 * @note Auto-sized tags:
 * - MercalliStreamKey_SmoothFrameMatrix: sizeof(MercalliStreamKey_FrameMatrix)
 * - MercalliStreamKey_RealFrameMatrix:   sizeof(MercalliStreamKey_FrameMatrix)
 * - MercalliStreamKey_MediaInfo:         sizeof(T_MercalliStreamMediaInfo)
 * - MercalliStreamKey_CameraIntrinsic:   sizeof(T_MercalliStreamCameraIntrinsic)
 * - MercalliStreamKey_SettingsB:         sizeof(T_MercalliStreamSettingsB)
 */
MERCALLISDK_API(HRESULT, MercalliGetDataStreamTag)(
  /* in  */ const T_GlobalMotionPath*  pGMP,
  /* in  */ const T_MercalliStreamTag* pSourceTags,
  /* out */       T_MercalliStreamTag* pTargetTags,
  /* in  */       SINT32               count);

/* Calculate the required byte-size of the T_MercalliDataStream
 * @param pSize -- Result byte-size
 * @param pTags --
 * @param count -- Number of pTags[]
 * @return HResult
 */
MERCALLISDK_API(HRESULT, MercalliCalcDataStreamSize)(
  /* out */       UINT64*              pSize,
  /* in  */ const T_MercalliStreamTag* pTags,
  /* in  */       SINT32               count);

/* Fill T_MercalliDataStream.
 * @param pGMP       --
 * @param pBuffer    --
 * @param bufferSize --
 * @param pTags      --
 * @param count      -- Number of pTags[]
 * @return HResult
 *
 * @list Tags
 * - MercalliStreamKey_SmoothFrameMatrix: Current GMP frame-data will be copied
 * - MercalliStreamKey_RealFrameMatrix:   Current GMP frame-data will be copied
 * - MercalliStreamKey_MediaInfo:         Current GMP media-info will be copied
 * - MercalliStreamKey_CameraIntrinsic:   Current GMP cam-info will be copied
 * - MercalliStreamKey_SettingsB:         Copy not possible. Item-data be clear.
 *
 */
MERCALLISDK_API(HRESULT, MercalliBuildDataStream)(
  /* in  */ const T_GlobalMotionPath*  pGMP,
  /* out */       void*                pBuffer,
  /* in  */       UINT64               bufferSize,
  /* in  */ const T_MercalliStreamTag* pTags,
  /* in  */       SINT32               count);


#pragma region Create/Free MercalliDataStream
/* Allocate full initialized DataStream.
 * @param pGMP  --
 * @param pTags --
 * @param count -- Number of pTags[]
 * @return NULL by Error. Free the DataStream by ref MercalliFreeDataStream()
 */
__STATIC_INLINE
T_MercalliDataStream* MercalliCreateDataStream(
  /* in  */ const T_GlobalMotionPath*  pGMP,
  /* in  */ const T_MercalliStreamTag* pTags,
  /* in  */       SINT32               count,
  /* out */       HRESULT*             pHR = NULL)
{
  assert(pGMP != NULL && pTags != NULL && count > 0);
  HRESULT hr = 0;
  T_MercalliDataStream* Stream = NULL;
  T_MercalliStreamTag* TargetTags = NULL;
  UINT64 Size = 0;

  TargetTags = new T_MercalliStreamTag[count];
  if(TargetTags == NULL)
    hr = E_OUTOFMEMORY;

  if(SUCCEEDED(hr))
    hr = MercalliGetDataStreamTag(pGMP, pTags, TargetTags, count);
  if(SUCCEEDED(hr))
    hr = MercalliCalcDataStreamSize(&Size, TargetTags, count);
  if(SUCCEEDED(hr))
  {
    Stream = (T_MercalliDataStream*)malloc( (size_t)Size );
    if(Stream == NULL)
      hr = E_OUTOFMEMORY;
  }
  if(SUCCEEDED(hr))
    hr = MercalliBuildDataStream(pGMP, Stream, Size, TargetTags, count);


  if(FAILED(hr))
  {
    if(Stream != NULL)
    {
      free(Stream);
      Stream = NULL;
    }
  }

  if(pHR != NULL)
    *pHR = hr;

  delete [] TargetTags;
  return Stream;
}

/* Release memory block was allocated by @ref MercalliCreateDataStream()
 * @param p -- from @ref MercalliCreateDataStream()
 */
__STATIC_INLINE
void MercalliFreeDataStream(
  /* in  */ T_MercalliDataStream* p)
{
  if(p != NULL)
  {
    assert(p->Magic == MercalliMagix_DataStream);
    free(p);
  }
}
#pragma endregion

#pragma region Get Item of MercalliDataStream
/* Get a DataStream item
 * @param pStream --
 * @param key     -- Item-Key like: MercalliStreamKey_XY
 * @param pPtr    -- optional, Pointer to Data
 * @param pSize   -- optional, Size of Data
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetDataStreamItem(
  /* in  */       T_MercalliDataStream* pStream,
  /* in  */ const GUID&                 key,
  /* out */       void**                pPtr,
  /* out */       UINT64*               pSize)
{
  HRESULT hr = 0;
  if(pStream == NULL)
    return E_POINTER;
  if(pStream->Magic != MercalliMagix_DataStream)
    return E_FAIL;

  SINT32 Index;
  for(Index = pStream->ItemCount; --Index >= 0; )
  {
    if(pStream->Items[Index].Key == key)
      break;
  }

  if(Index < 0)
    hr = E_FAIL;
  else
  {
    if(pPtr != NULL)
      *pPtr = (void*)((UINT_PTR)pStream + pStream->Items[Index].Offset);

    if(pSize != NULL)
      *pSize = pStream->Items[Index].Size;
  }

  return hr;
}

/* Template-Function to avoid too much inlining
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
template<typename T>
HRESULT MercalliGetDataStream(
  /* in  */       T_MercalliDataStream* pStream,
  /* in  */ const GUID&                 key,
  /* out */       T**                   pPtr,
  /* out */       UINT64*               pSize = NULL)
{
  HRESULT hr = 0;
  if(pStream == NULL || pPtr == NULL)
    return E_POINTER;

  hr = MercalliGetDataStreamItem(pStream, key, (void**)pPtr, pSize);
  return hr;
}

/* get T_MercalliStreamMediaInfo
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetStreamMediaInfo(
  /* in  */ T_MercalliDataStream*       pStream,
  /* out */ T_MercalliStreamMediaInfo** pPtr)
{
  HRESULT hr;
  UINT64 Size;

  /* Use (void**) to reduce the instancing of the template-function */
  hr = MercalliGetDataStream(pStream, MercalliStreamKey_MediaInfo, (void**)pPtr, &Size);
  if(SUCCEEDED(hr))
  {
    assert(sizeof(T_MercalliStreamMediaInfo) == Size);
    if(sizeof(T_MercalliStreamMediaInfo) != Size)
      return E_FAIL;
  }
  return hr;
}

/* get T_MercalliStreamCameraIntrinsic
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetStreamCameraIntrinsic(
  /* in  */ T_MercalliDataStream*             pStream,
  /* out */ T_MercalliStreamCameraIntrinsic** pPtr)
{
  HRESULT hr;
  UINT64 Size;

  /* Use (void**) to reduce the instancing of the template-function */
  hr = MercalliGetDataStream(pStream, MercalliStreamKey_CameraIntrinsic, (void**)pPtr, &Size);
  if(SUCCEEDED(hr))
  {
    assert(sizeof(T_MercalliStreamCameraIntrinsic) == Size);
    if(sizeof(T_MercalliStreamCameraIntrinsic) != Size)
      return E_FAIL;
  }
  return hr;
}

/* get T_MercalliStreamFrameMatrixKey
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetStreamSmoothFrameMatrix(
  /* in  */ T_MercalliDataStream*            pStream,
  /* out */ T_MercalliStreamFrameMatrixKey** pPtr)
{
  HRESULT hr;
  UINT64 Size;

  /* Use (void**) to reduce the instancing of the template-function */
  hr = MercalliGetDataStream(pStream, MercalliStreamKey_SmoothFrameMatrix, (void**)pPtr, &Size);
  if(SUCCEEDED(hr))
  {
    UINT64 ReqSize = sizeof(T_MercalliStreamFrameMatrixKey) + sizeof(T_MercalliStreamFrameMatrix) * ((*pPtr)->Count -1 );
    assert(ReqSize == Size);
    if(ReqSize != Size)
      return E_FAIL;
  }
  return hr;
}

/* get T_MercalliStreamFrameMatrixKey
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetStreamRealFrameMatrix(
  /* in  */ T_MercalliDataStream*            pStream,
  /* out */ T_MercalliStreamFrameMatrixKey** pPtr)
{
  HRESULT hr;
  UINT64 Size;

  /* Use (void**) to reduce the instancing of the template-function */
  hr = MercalliGetDataStream(pStream, MercalliStreamKey_RealFrameMatrix, (void**)pPtr, &Size);
  if(SUCCEEDED(hr))
  {
    UINT64 ReqSize = sizeof(T_MercalliStreamFrameMatrixKey) + sizeof(T_MercalliStreamFrameMatrix) * ((*pPtr)->Count -1 );
    assert(ReqSize == Size);
    if(ReqSize != Size)
      return E_FAIL;
  }
  return hr;
}

/* get T_MercalliStreamSettingsB
 * @param pStream --
 * @param key     --
 * @param pPtr    --
 * @param pSize   --
 * @return HResult
 */
__STATIC_INLINE
HRESULT MercalliGetStreamSettingsB(
  /* in  */ T_MercalliDataStream*       pStream,
  /* out */ T_MercalliStreamSettingsB** pPtr)
{
  HRESULT hr;
  UINT64 Size;

  /* Use (void**) to reduce the instancing of the template-function */
  hr = MercalliGetDataStream(pStream, MercalliStreamKey_SettingsB, (void**)pPtr, &Size);
  if(SUCCEEDED(hr))
  {
    assert(sizeof(T_MercalliStreamSettingsB) == Size);
    if(sizeof(T_MercalliStreamSettingsB) != Size)
      return E_FAIL;
  }
  return hr;
}

#pragma endregion



//END DataStream Functions
#pragma endregion

#pragma region Unjello Functions

/* Delete a via @ref MercalliCreateUnjello() created Unjello-Instance.
 */
MERCALLISDK_API(HRESULT, MercalliDeleteUnjello)(
  /* in  */ T_Unjello* pUJ);

/* Create a new and empty Unjello Instance.
 * @param fieldOrder -- (FieldFormat_Enum)
 * @param par        -- PixelAspectRatio: e.g.: 1.06=PAL
 * @param rate       -- Frame rate, 25=PAL, 29.97=NTSC, etc.
 * @return The Instance must free via @ref MercalliDeleteUnjello()
 *         NULL in case of fail.
 */
MERCALLISDK_API(HRESULT, MercalliCreateUnjello)(
  /* out */ T_Unjello** ppUJ,
  /* in  */ SINT32  width,
  /* in  */ SINT32  height,
  /* in  */ SINT32  fieldOrder,
  /* in  */ FLOAT32 par,
  /* in  */ FLOAT32 rate );


/* Analyzes a frame range and estimates shutter speed.
 * The values are used by @ref MercalliUnjelloRenderFrame() depending on @ref MercalliSetUnjelloEnableAutoShutterSpeed().
 * @note The frame range must be set using @ref MercalliSetUnjelloFrameNumberRange() beforehand.
 * @param pUJ              -- Unjello instance
 * @param pProgress        -- Optional progress callback (can be NULL)
 * @param startFrameNumber -- First required render frame
 * @param endFrameNumber   -- Last required render frame plus one
 * @return HRESULT
 *
 * @warn The UnjelloFrameCallback function is used by multiple threads concurrently.
 *
 * @note UnjelloFrameCallback required formats:
 * - FMT_Gray8
 * - FMT_BGR8
 * - FMT_BGRA8
 */
MERCALLISDK_API(HRESULT, MercalliEstimateUnjelloShutterSpeed)(
  /* in  */ T_Unjello*              pUJ,
  /* in  */ UnjelloProgressCallback pProgress);

/* Render Frame
 * @return HResult
 * - E_PENDING: Is called recursive. Possible reason:
 *              MercalliUnjelloRenderFrame -> UnjelloFrameCallback -> MercalliUnjelloRenderFrame
 *
 *@notice
 * Don't call @ref MercalliSetUnjelloCustomData() or @ref MercalliSetUnjelloFrameCallback()
 * after UnjelloRenderFrame().
 *
 * @important
 * The UnjelloFrameCallback function is used by massive multi-threads.
 *
 * @list UnjelloFrameCallback required Formats:
 * - FMT_Gray8
 * - FMT_BGR8
 * - FMT_BGRA8
 * - <Current Render Format>
 *
 * @list Supported Formats:
 * - FMT_BGR8
 * - FMT_BGR16
 * - FMT_BGRA8
 * - FMT_BGRA16
 * - FMT_VUYA8
 * - FMT_VUYA16
 * - FMT_VUY8
 * - FMT_VUY16
 * - FMT_ARGB8
 * - FMT_ARGB16
 * - FMT_AYUV8
 * - FMT_AYUV16
 * - FMT_VUY8
 * - FMT_VUY16
 */
MERCALLISDK_API(HRESULT, MercalliUnjelloRenderFrame)(
  /* in  */       T_Unjello*                 pUJ,
  /* in  */       SINT64                     frameNumber,
  /* in  */ const T_UnjelloRenderFrameParam* pParam );



//END Unjello Functions
#pragma endregion

#pragma region Unjello Get Functions

/* Get Unjello value
 * @param pUJ        --
 * @param type       -- (GetUJValueType_Enum)
 * @param pArg       -- Input argument, depending on type
 * @param argSize    -- byte size of pArg
 * @param pResult    -- Result, depending on type
 * @param resultSize -- byte size of pResult
 * @return HResult
 */
MERCALLISDK_API(HRESULT, MercalliGetUnjelloValue)(
  /* in  */ const T_Unjello* pUJ,
  /* in  */       SINT32     type,
  /* in  */ const void*      pArg,
  /* in  */       SINT32     argSize,
  /* out */       void*      pResult,
  /* in  */       SINT32     resultSize);


#pragma region MercalliGetUnjelloCustomData
/*! Get custom data, that was setter by @ref MercalliSetUnjelloData()
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloCustomData(
  /* in  */ const T_Unjello* pUJ,
  /* out */       UINT_PTR*  pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_CustomData,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloCustomData<>
/* MercalliGetUnjelloCustomData with type.
 */
template<typename T>
HRESULT MercalliGetUnjelloCustomData(
  /* in  */ const T_Unjello* pUJ,
  /* out */       T**        ppValue)
{
  assert(ppValue != NULL);
  HRESULT hr;
  UINT_PTR Data;
  hr = MercalliGetUnjelloCustomData(pUJ, &Data);
  *ppValue = (T*)Data;
  return hr;
}
#pragma endregion

#pragma region MercalliGetUnjelloMethod
/*! Get Method
 * @param value -- (UnjelloMethodType_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloMethod(
  /* in  */ const T_Unjello* pUJ,
  /* out */       UINT32*    pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_Method,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloEnableAutoShutterSpeed
/*! Get Auto Shutter-Speed enabled/disabled
 * @param value
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloEnableAutoShutterSpeed(
  /* in  */ const T_Unjello* pUJ,
  /* out */       BOOL*      pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_EnableAutoShutterSpeed,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloAverageShutterSpeed
/*! Get avg. Rolling-Shutter Sensor read-out Speed (after @ref MercalliEstimateUnjelloShutterSpeed())
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloAverageShutterSpeed(
  /* in  */ T_Unjello* pUJ,
  /* out */ FLOAT32*   pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_AverageShutterSpeed,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Option
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloShutterSpeed
/*! Get Rolling-Shutter Sensor read-out Speed.
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloShutterSpeed(
  /* in  */ T_Unjello* pUJ,
  /* out */ FLOAT32*   pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_ShutterSpeed,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Option
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloFrameNumberRange
/*! Get Frame-Range
 * @param startFrameNumber
 * @param endFrameNumber
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloFrameNumberRange(
  /* in  */ T_Unjello* pUJ,
  /* out */ SINT64*    pStartFrameNumber,
  /* out */ SINT64*    pEndFrameNumber)
{
  HRESULT hr;
  SINT64 Value[2] = {0};
  hr = MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_FrameNumberRange,
    NULL, 0,             // Arguments
    Value, sizeof(Value) // Option
  );
  if(pStartFrameNumber != NULL)
    *pStartFrameNumber = Value[0];
  if(pEndFrameNumber != NULL)
    *pEndFrameNumber = Value[1];
  return hr;
}
#pragma endregion

#pragma region MercalliEstimateUnjelloShutterSpeedRequired
/*! Checks if @ref MercalliEstimateUnjelloShutterSpeed() is required
 * @param value -- (UnjelloMethodType_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliEstimateUnjelloShutterSpeedRequired(
  /* in  */ const T_Unjello* pUJ,
  /* out */       BOOL*      pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_EstimateShutterSpeedRequired,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliGetUnjelloBorderFillMode
/*! Get BorderFillMode
 * @param value -- (UnjelloMethodType_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloBorderFillMode(
  /* in  */ const T_Unjello*                  pUJ,
  /* out */       UnjelloBorderFillMode_Enum* pValue)
{
  HRESULT hr;
  UINT32 Tmp;
  if(pValue == NULL)
    return E_POINTER;
  hr = MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_BorderFillMode,
    NULL, 0,          // Arguments
    &Tmp, sizeof(Tmp) // Result
  );
  *pValue = (UnjelloBorderFillMode_Enum)Tmp;
  return hr;
}
#pragma endregion

#pragma region MercalliGetUnjelloRemapMode
/*! Get RemapMode
 * @param value -- (UnjelloMethodType_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloRemapMode(
  /* in  */ const T_Unjello*       pUJ,
  /* out */       MRemapMode_Enum* pValue)
{
  HRESULT hr;
  UINT32 Tmp;
  if(pValue == NULL)
    return E_POINTER;
  hr = MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_RemapMode,
    NULL, 0,          // Arguments
    &Tmp, sizeof(Tmp) // Result
  );
  *pValue = (MRemapMode_Enum)Tmp;
  return hr;
}
#pragma endregion

#pragma region MercalliGetUnjelloMethodAvailable
/*! Get MethodAvailable
 * @param method -- (UnjelloMethodType_Enum)
 * @param value  -- (as BOOL)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliGetUnjelloMethodAvailable(
  /* in  */ const T_Unjello* pUJ,
  /* int */       UINT32     method,
  /* out */       UINT32*    pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_MethodAvailable,
    &method, sizeof(method), // Arguments
    pValue, sizeof(*pValue)  // Result
  );
}
#pragma endregion

#pragma region MercalliUnjelloFlushCount
/*! Get amount of Flush calls (only for debugging)
 * @param value --
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliUnjelloFlushCount(
  /* in  */ const T_Unjello* pUJ,
  /* out */       UINT32*    pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_FlushCount,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliUnjelloEnableFrameCallback
/*!
 * @param value --
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliUnjelloEnableFrameCallback(
  /* in  */ const T_Unjello* pUJ,
  /* out */       BOOL*      pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_EnableFrameCallback,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliUnjelloLatencyRenderEnabled
/*!
 * @param value --
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliUnjelloLatencyRenderEnabled(
  /* in  */ const T_Unjello* pUJ,
  /* out */       BOOL*      pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_LatencyRenderEnabled,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliUnjelloLatencyRenderPossible
/*!
 * @param value --
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliUnjelloLatencyRenderPossible(
  /* in  */ const T_Unjello* pUJ,
  /* in  */       SINT64     frameNumber,
  /* out */       BOOL*      pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_LatencyRenderPossible,
    &frameNumber, sizeof(frameNumber), // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion

#pragma region MercalliUnjelloLatencyFrameCount
/*! Get the Latency frame count used by UJRSYS_LatencyX.
 * @param value --
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliUnjelloLatencyFrameCount(
  /* in  */ const T_Unjello* pUJ,
  /* out */       UINT32*    pValue)
{
  return MercalliGetUnjelloValue
  (
    pUJ,
    UJVTyp_LatencyFrameCount,
    NULL, 0,                // Arguments
    pValue, sizeof(*pValue) // Result
  );
}
#pragma endregion




//END Unjello Get Functions
#pragma endregion

#pragma region Unjello Set Functions

/* Set Unjello value
 * @param pUJ        --
 * @param type       -- (GetUJValueType_Enum)
 * @param pArg       -- Input argument, depending on type
 * @param argSize    -- byte size of pArg
 * @param pOption    -- additional options, depending on type
 * @param optionSize -- byte size of pOption
 * @return HResult
 */
MERCALLISDK_API(HRESULT, MercalliSetUnjelloValue)(
  /* in  */       T_Unjello* pUJ,
  /* in  */       SINT32     type,
  /* in  */ const void*      pArg,
  /* in  */       SINT32     argSize,
  /* in  */ const void*      pOption,
  /* in  */       SINT32     optionSize);


#pragma region MercalliSetUnjelloCustomData
/*! Set custom data. That can get by @ref MercalliGetUnjelloData()
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloCustomData(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT_PTR   value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_CustomData,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloFrameCallback
/*! Set UnjelloFrame-Callback
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloFrameCallback(
  /* in  */ T_Unjello*           pUJ,
  /* in  */ UnjelloFrameCallback value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_UnjelloFrameCallback,
    NULL, 0,             // Arguments
    (void*)value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloShutterSpeed
/*! Set Rolling-Shutter Sensor read-out Speed.
 * @param value -- [1.0 ...]
 *  - 1.0 -- Sensor read-out time equal the frame display time
 *  - 2.0 -- Sensor read-out time is half of the frame display time
 *  - etc.
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloShutterSpeed(
  /* in  */ T_Unjello* pUJ,
  /* in  */ FLOAT32    value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_ShutterSpeed,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloMethod
/*! Enable Auto Shutter-Speed detection
 * @param value -- (UnjelloMethodType_Enum)
 * @return HResult
 *
 * @note
 *   UJMthTyp_PTek_OF requires GPU operations.
 *   The fallback UJMthTyp_PTek_FP is used instead if no GPU is available
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloMethod(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_Method,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloFrameNumberRange
/*! Set Frame-Range. That is required for some operations.
 *  Like: @ref MercalliEstimateUnjelloShutterSpeed(), @ref MercalliUnjelloRenderFrame()
 * @param startFrameNumber
 * @param endFrameNumber
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloFrameNumberRange(
  /* in  */ T_Unjello* pUJ,
  /* in  */ SINT64     startFrameNumber,
  /* in  */ SINT64     endFrameNumber)
{
  SINT64 Value[2] = {startFrameNumber, endFrameNumber};
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_FrameNumberRange,
    NULL, 0,             // Arguments
    Value, sizeof(Value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloEnableAutoShutterSpeed
/*! Enable/Disable Auto Shutter-Speed detection
 * @param value
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloEnableAutoShutterSpeed(
  /* in  */ T_Unjello* pUJ,
  /* in  */ BOOL       value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_EnableAutoShutterSpeed,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloBorderFillMode
/*! Set BorderFillMode
 * @param value -- (enum UnjelloBorderFillMode_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloBorderFillMode(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_BorderFillMode,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloRemapMode
/*! Set RemapMode
 * @param value -- (enum MRemapMode_Enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloRemapMode(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_RemapMode,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloSplitscreenView
/*! Set SplitscreenView
 * @param value -- (enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloSplitscreenView(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_SplitscreenView,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloBrandingView
/*! Set BrandingView
 * @param value -- (enum)
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloBrandingView(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_BrandingView,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloGpuDeviceCount
/*! Set Numbers of GPU Devices.
 * @param value -- [0,1,2 ... ]
 * - (0): special case
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloGpuDeviceCount(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_GpuDeviceCount,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloEnableFrameCallback
/*! Enable/Disable the using of @ref MercalliSetUnjelloFrameCallback defined callback.
 * Is disabled, then @ref MercalliUnjelloRenderFrame must used with T_UnjelloRenderFrameParam::Src
 * Default: true
 * @param value
 * @return HResult
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloEnableFrameCallback(
  /* in  */ T_Unjello* pUJ,
  /* in  */ BOOL       value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_EnableFrameCallback,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion

#pragma region MercalliSetUnjelloRenderSystem
/*!
 * @param value -- (UnjelloRenderSystem_Enum)
 * @return HResult
 *
 * @note
 * Only possible directly after @ref MercalliCreateUnjello()
 */
__STATIC_INLINE HRESULT MercalliSetUnjelloRenderSystem(
  /* in  */ T_Unjello* pUJ,
  /* in  */ UINT32     value)
{
  return MercalliSetUnjelloValue
  (
    pUJ,
    UJVTyp_RenderSystem,
    NULL, 0,              // Arguments
    &value, sizeof(value) // Option
  );
}
#pragma endregion




//END Unjello Set Functions
#pragma endregion


//END Functions
#pragma endregion


#pragma region Epilog

#ifdef _MSC_VER
#pragma warning ( pop )
#pragma managed(pop)
#endif


#endif
#pragma endregion
