#ifndef __MERCALLICLI_VIDEOUNJELLO_H
#define __MERCALLICLI_VIDEOUNJELLO_H
/*******************************************************************
$CRT 13 Jul 2014 : hb

$AUT Holger Burkarth
$DAT >>VideoUnjello.h<< 15 Okt 2024  10:00:29 - (c) proDAD
*******************************************************************/

/*! \brief Unjello processing for removing CMOS sensor distortions.
 */

#pragma region Flowchart
 /*!
 ===================================================================
 =                    Unjello Process Overview                     =
 ===================================================================

                            /-------------/
                           /  Open Video /
                          /-------------/
                                  v
                   +--------------+---------------+
 +------>----------+                              +-------<--------+
 |                 v                              v                |
 |   +-------------------------+    +-------------------------+    |
 |   || Instant Unjello       ||    || Reflected Unjello     ||    |
 |   |-------------------------|    |-------------------------|    |
 |   | - const Sensor Speed    |    | - variable Sensor Speed |    |
 |   | - Real-Time             |    | - auto  Sensor Speed    |    |
 |   |                         |    | - Analysis required     |    |
 |   +-----+-------------+-----+    +----+--------------+-----+    |
 |         |             |               |              |          |
 |         v             v               v              v          |
 |   +-----------+  +----------+    +-----------+  +----------+    |
 |   | Playback  |  || Export ||    | Playback  |  || Export ||    |
 |   +-----+-----+  +----+-----+    +----+------+  +----+-----+    |
 |         v             v               v              v          |
 |         |             |               |              |          |
 +---<-----+-------------+               +--------------+---->-----+






 ===================================================================
 =                     Instant Unjello                             =
 ===================================================================


             /------------------/
            /    Open Video    /
           /------------------/
                     |
                     +------------------------<--------------------+
                     |                                             |
                     v                                             |
            +-------------------+                                  |
            ||  Configuration  ||                                  |
            +-------------------+                                  |
                     |                                             |
                     v                                             |
   +---------------------------------------------+                 |
   | Update <15>                                 |                 |
   |---------------------------------------------|                 |
   | - SetUnjelloFrameNumberRange                |                 ^
   | - SetUnjelloEnableAutoShutterSpeed( false ) |                 |
   | - SetUnjelloShutterSpeed( Speed )           |                 |
   | - etc.                                      |                 |
   +-----------------+---------------------------+                 |
                     |                                             |
     +------>--------+                                             |
     |               v                                             |
     |      +-------------------+                                  |
     |      | Render Frame <12> |                                  |
     |      +-------------------+                                  |
     |               v                                             |
     +-----<--------/ \---------->---------------------------------+





 ===================================================================
 =                     Reflected Unjello                           =
 ===================================================================

             /------------------/
            /    Open Video    /
           /------------------/
                     |
                     +------------------------<-----------------------+
                     |                                                |
                     v                                                |
            +-------------------+                                     |
            ||  Configuration  ||                                     |
            +-------------------+                                     |
                     |                                                |
                     v                                                |
   +---------------------------------------------+                    |
   | Update <15>                                 |                    |
   |---------------------------------------------|                    |
   | - SetUnjelloFrameNumberRange                |                    |
   | - SetUnjelloEnableAutoShutterSpeed          |                    |
   | - SetUnjelloShutterSpeed                    |                    |
   | - etc.                                      |                    |
   +-----------------+---------------------------+                    |
                     |                                                ^
                     v                                                |
         +------------------------+ Yes  +------------------------+   |
         |  If analysis required? |----->|| Perform Analysis<16> ||   |
         +-----------+------------+      +---------+--------------+   |
                     | No                          v                  |
                     +-------------<---------------+                  |
     +------>--------+                                                |
     |               v                                                |
     |      +-------------------+                                     |
     |      | Render Frame <12> |                                     |
     |      +-------------------+                                     |
     |               v                                                |
     +-----<--------/ \---------->------------------------------------+










 ===============================================================
 =                  Configuration                              =
 ===============================================================

   +-------------------------+  Yes +----------------+
   | Video settings changed? |----->| Delete Unjello |
   | Such: FieldOrder, Rate, |      +----------------+
   | Size, Start-Endtime, ...|             |
   +-------------------------+             |
              |                            |
              v                            v
   +-------------------------+  Yes +----------------+
   | If Unjello == NULL      |----->| Create Unjello |
   +-------------------------+      +----------------+
              |                            v
              +-------------<--------------+
              v
   +---------------------------+
   | Setup  <14>               |
   |---------------------------|
   | - SetUnjelloCustomData    |
   | - SetUnjelloFrameCallback |
   | - ect.                    |
   +---------------------------+
              :
              v






 ===============================================================
 =                  Analysis Video <16>                        =
 ===============================================================
                            +------->------+
           |                |              |<--------------+
           |                ^              v               |
           |                |  +-----------+------------+  |
           v                |  |UnjelloFrameCallback<13>|  |
+-----------------------+   |  +-----------+------------+  |
| Estimate Shutter Speed -->/              |               |
| <17>                  <---\              v               ^
+----------+------------+   |  +------------------------+  |
           |                |  |UnjelloPushFrameCallback|  |
           |                |  +-----------+------------+  |
           |                ^              |               |
           |                |              v               |
           |                +--------<----/ \----->--------+
           v






 ===================================================================
 =                        Export Process <11>                      =
 ===================================================================

   /------------------/
  /  Create Encoder  /
 /------------------/
          |             +----------------------------+
          |             |     Render Frame <12>      |
          |             |----------------------------|  +------->------+
          |             |  +----------------------+  |  |              |<--------------+
          |          +---> |  Create Frame Buffer |  |  ^              v               |
          |          |  |  +----------------------+  |  |  +-----------+------------+  |
          v          |  |              v             |  |  |UnjelloFrameCallback<13>|  |
 +----------------+  ^  |  +----------------------+  |  |  +-----------+------------+  |
 | For Each Frame +--:  |  | Render Unjello into  +---->/              |               |
 +--------+-------+  ^  |  | Frame-Buffer         <-----\              v               ^
          |          |  |  +----------------------+  |  |  +------------------------+  |
          |          |  |              v             |  |  |UnjelloPushFrameCallback|  |
          |          |  |  +----------------------+  |  |  +-----------+------------+  |
          |          +---< |  Encode Frame Buffer |  |  ^              |               |
          |             |  +----------------------+  |  |              v               |
          |             |                            |  +--------<----/ \----->--------+
          v             +----------------------------+
   /-----------------/
  / Close Encoder   /
 /-----------------/



 Footer:
 <11>  see VideoUnjelloExport()
 <12>  see VideoUnjelloRenderFrame()
 <13>  see UnjelloFrame()
 <14>  see VideoUnjelloSetup()
 <15>  see VideoUnjelloUpdate()
 <16>  see VideoUnjelloAnalysis()
 <17>  see MercalliEstimateUnjelloShutterSpeed()
  */
#pragma endregion

#pragma region CVideoUnjelloParam
  /*! \brief Structure for configuring Unjello processing parameters.
   */
struct CVideoUnjelloParam
{
  bool                      Verbose{};                    /*!< Enable verbose output */
  T_Unjello*                Unjello{};                    /*!< Unjello processing instance */
  IVideoDecoder*            VideoDecoder{};               /*!< Source video decoder */
  IVideoEncoder*            VideoEncoder{};               /*!< Target video encoder */
  CMediaInfo                SourceMediaInfo{};            /*!< Source video metadata */
  SINT32                    SourceStartFrameNumber{};     /*!< Start frame number */
  SINT32                    SourceEndFrameNumber{};       /*!< End frame number */
  bool                      AutoShutterSpeed{ true };     /*!< Enable automatic shutter speed */
  bool                      EstimateShutterSpeed{ true }; /*!< Enable shutter speed estimation */
  FLOAT32                   ShutterSpeed{ 1 };            /*!< Shutter speed value */
  UnjelloMethodType_Enum    Method{ UJMthTyp_PTek_OF_Fast }; /*!< Unjello processing method */
  UnjelloRenderSystem_Enum  RenderSystem{ UJRSYS_SyncPreload }; /*!< Rendering system */

  /*! \brief Destructor for CVideoUnjelloParam.
   */
  ~CVideoUnjelloParam();

  /*! \brief Constructor for CVideoUnjelloParam.
   */
  CVideoUnjelloParam();

  /*! \brief Enables or disables shutter speed estimation.
   * \param value True to enable, false to disable
   * \return HRESULT indicating success (always 0)
   */
  HRESULT EnableEstimateShutterSpeed(bool value)
  {
    EstimateShutterSpeed = value;
    return 0;
  }

  /*! \brief Enables or disables automatic shutter speed adjustment.
   * \param value True to enable, false to disable
   * \return HRESULT indicating success (always 0)
   */
  HRESULT EnableAutoShutterSpeed(bool value)
  {
    AutoShutterSpeed = value;
    return 0;
  }

  /*! \brief Sets the shutter speed value.
   * \param value Shutter speed
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetShutterSpeed(float value)
  {
    ShutterSpeed = value;
    return 0;
  }

  /*! \brief Sets the Unjello processing method.
   * \param value Processing method (UnjelloMethodType_Enum)
   * \return HRESULT indicating success (always 0)
   */
  HRESULT SetMethod(UnjelloMethodType_Enum value)
  {
    Method = value;
    return 0;
  }
};
#pragma endregion

#pragma region Functions
/*! \brief Sets up important properties for Unjello processing.
 * \param param Unjello processing parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoUnjelloSetup(CVideoUnjelloParam& param);

/*! \brief Updates variable properties and performs analysis if required.
 * \param param Unjello processing parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoUnjelloUpdate(CVideoUnjelloParam& param);

/*! \brief Renders an Unjello-processed frame.
 * \param param        Unjello processing parameters
 * \param frameNumber  Frame index
 * \param field        Field type for interlaced video
 * \param pTargetFrame Target video frame (input/output)
 * \return HRESULT indicating success or failure
 */
HRESULT VideoUnjelloRenderFrame(
  /* in  */ CVideoUnjelloParam& param,
  /* in  */ SINT64       frameNumber,
  /* in  */ field_type   field,
  /* i/o */ IVideoFrame* pTargetFrame);

/*! \brief Exports an Unjello-processed video by rendering each frame.
 * \param param Unjello processing parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoUnjelloExport(CVideoUnjelloParam& param);

/*! \brief Performs analysis to estimate sensor shutter speed if required.
 * \param param Unjello processing parameters
 * \return HRESULT indicating success or failure
 */
HRESULT VideoUnjelloAnalysis(CVideoUnjelloParam& param);

/*! \brief Prints Unjello options to the console.
 * \param param Unjello processing parameters
 */
void PrintUnjelloOptions(CVideoUnjelloParam& param);
#pragma endregion

#pragma region Epilog
#endif // __MERCALLICLI_VIDEOUNJELLO_H

#pragma endregion