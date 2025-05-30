@REM
@REM $CRT 13 Jul 2014 : hb
@REM
@REM $AUT Holger Burkarth
@REM $DAT >>WinValidateMercalliCli.bat<< 28 Mai 2025  10:54:19 - (c) proDAD

@ECHO OFF

@REM --- Setup ---
SET SDir=%~dp0
SET Mercalli="MercalliCli_static.exe"
SET MercalliDir="..\bin\x64\release\"

if not exist "%MercalliDir%\%Mercalli%" (
  SET MercalliDir="..\bin\17.0\x64\release\"
) 

SET MediaDir=%SDir%media\
SET OutDir=%SDir%out\

SET MediaCmosVibration="%MediaDir%CmosVibration.mp4"
SET MediaGoProWide="%MediaDir%CmosGoPro3Wide.mp4"
SET MediaFields="%MediaDir%CmosTopField.mp4"
SET MediaTimeRange="%MediaDir%CmosTopField.mp4"
SET MediaHyperlapse="%MediaDir%CmosGoProFlight.mp4"

SET OutTmp="%OutDir%\tmp.mp4"

SET OutFisheyeForensic="%OutDir%\FisheyeForensic.mp4"
SET OutFieldsForensic="%OutDir%\FieldsForensic.mp4"

SET OutFisheye="%OutDir%\Fisheye.mp4"
SET OutFields="%OutDir%\Fields.mp4"
SET OutReflectedUnjelloOF="%OutDir%\ReflectedUnjelloOF.mp4"
SET OutInstantUnjelloOF="%OutDir%\InstantUnjelloOF.mp4"
SET OutInstantUnjelloFP="%OutDir%\InstantUnjelloFP.mp4"
SET OutFieldsInstantUnjelloOF="%OutDir%\FieldsInstantUnjelloOF.mp4"
SET OutFieldsInstantUnjelloFP="%OutDir%\FieldsInstantUnjelloFP.mp4"
SET OutVibration="%OutDir%\Vibration.mp4"
SET OutTimeRange="%OutDir%\TimeRange.mp4"
SET OutHyperlapse3="%OutDir%\Hyperlapse3.mp4"
SET OutHyperlapse8="%OutDir%\Hyperlapse8.mp4"



@REM --- Prepare ---
cd %SDir%
cd %MercalliDir%

if not exist "%OutDir%" mkdir "%OutDir%"


@REM --- Perform Operations ---

echo.
echo.
echo ==================================
echo == Stabilize Fisheye Forensic   ==
echo ==================================
if not exist "%OutFisheyeForensic%" (
%Mercalli% --source "%MediaGoProWide%" --target "%OutFisheyeForensic%" --RS ON --Camera "GoPro Hero3+ Wide" --TranslationSmoothing 1.0 --ZRotationSmoothing 1.0 --BorderMode Forensic --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ==================================
echo == Stabilize Fisheye Standard   ==
echo ==================================
if not exist "%OutFisheye%" (
%Mercalli% --source "%MediaGoProWide%" --target "%OutFisheye%" --RS ON --Camera "GoPro Hero3+ Wide" --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Stabilize Interlace Forensic   ==
echo ====================================
if not exist "%OutFieldsForensic%" (
%Mercalli% --source "%MediaFields%" --target "%OutFieldsForensic%" --RS ON --TranslationSmoothing 0.95 --ZRotationSmoothing 0.95 --BorderMode Forensic --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Stabilize Interlace Standard   ==
echo ====================================
if not exist "%OutFields%" (
%Mercalli% --source "%MediaFields%" --target "%OutFields%" --RS ON --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ===============================
echo ==    Reflected Unjello OF   ==
echo ===============================
if not exist "%OutReflectedUnjelloOF%" (
%Mercalli% --source "%MediaCmosVibration%" --target "%OutReflectedUnjelloOF%" --EstimateShutterSpeed ON --AutoShutterSpeed ON --unjello
@if errorlevel 1 goto Error
)

echo.
echo.
echo ===============================
echo ==    Instant Unjello OF     ==
echo ===============================
if not exist "%OutInstantUnjelloOF%" (
%Mercalli% --source "%MediaCmosVibration%" --target "%OutInstantUnjelloOF%" --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
@if errorlevel 1 goto Error
)

echo.
echo.
echo ===============================
echo ==    Instant Unjello FP     ==
echo ===============================
if not exist "%OutInstantUnjelloFP%" (
%Mercalli% --source "%MediaCmosVibration%" --target "%OutInstantUnjelloFP%" --UJMth P-FP --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Instant Unjello OF (Interlace) ==
echo ====================================
if not exist "%OutFieldsInstantUnjelloOF%" (
%Mercalli% --source "%MediaFields%" --target "%OutFieldsInstantUnjelloOF%" --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Instant Unjello FP (Interlace) ==
echo ====================================
if not exist "%OutFieldsInstantUnjelloFP%" (
%Mercalli% --source "%MediaFields%" --target "%OutFieldsInstantUnjelloFP%" --UJMth P-FP --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
@if errorlevel 1 goto Error
)

echo.
echo.
echo ===============================
echo == Stabilize Unjello-Output  ==
echo ===============================
if not exist "%OutVibration%" (
%Mercalli% --source "%OutReflectedUnjelloOF%" --target "%OutVibration%" --Camera "GoPro Hero3+ Wide" --RS OFF --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Stabilize TimeRange Forensic   ==
echo ====================================
if not exist "%OutTimeRange%" (
%Mercalli% --source "%MediaTimeRange%" --target "%OutTimeRange%" --IN 4 --OUT 6 --RS ON --TranslationSmoothing 0.95 --ZRotationSmoothing 0.95 --BorderMode Forensic --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Hyperlapse3 (3 times faster)   ==
echo ====================================
if not exist "%OutHyperlapse3%" (
%Mercalli% --source "%MediaHyperlapse%" --target "%OutHyperlapse3%" --FrameStep 3 --StabiProfile HyperlapseX4  --RS ON --Camera "GoPro Hero3+ Wide" --stabilize
@if errorlevel 1 goto Error
)

echo.
echo.
echo ====================================
echo == Hyperlapse8 (8 times faster)   ==
echo ====================================
if not exist "%OutHyperlapse8%" (
%Mercalli% --source "%MediaHyperlapse%" --target "%OutHyperlapse8%" --FrameStep 8 --StabiProfile HyperlapseX10 --RS ON --Camera "GoPro Hero3+ Wide" --stabilize
@if errorlevel 1 goto Error
)





echo.
echo.
echo .........................
echo ..... Test Finished .....
echo .........................
echo Result Videos at: %OutDir%
echo.
pause
goto End

:Error
echo.
echo ++++++++++++++
echo +++ failed +++
echo ++++++++++++++
echo.
pause

:End

