#!/bin/bash
#
# $CRT 13 Jul 2014 : hb
#
# $AUT Holger Burkarth
# $DAT >>OsxValidateMercalliCli.sh<< 02 Aug 2014  10:28:23 - (c) proDAD


# --- Setup ---
SDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MercalliDir=$SDir/../bin/osx/Release/
##MercalliDir=$SDir/../AppMercalliCli/build/xcode/MercalliCliStatic/build/Release/
Mercalli=$MercalliDir/MercalliCli_static
# Mercalli=$MercalliDir/MercalliCli_dyn
MediaDir=$SDir/media/
OutDir=$SDir/out/

MediaCmosVibration=$MediaDir/CmosVibration.mp4
MediaGoProWide=$MediaDir/CmosGoPro3Wide.mp4
MediaFields=$MediaDir/CmosTopField.mp4

OutTmp=$OutDir/tmp.mp4


OutFisheyeAggressive=$OutDir/FisheyeAggressive.mp4
OutFieldsAggressive=$OutDir/FieldsAggressive.mp4

OutFisheye=$OutDir/Fisheye.mp4
OutFields=$OutDir/Fields.mp4
OutReflectedUnjelloOF=$OutDir/ReflectedUnjelloOF.mp4
OutInstantUnjelloOF=$OutDir/InstantUnjelloOF.mp4
OutInstantUnjelloFP=$OutDir/InstantUnjelloFP.mp4
OutVibration=$OutDir/Vibration.mp4


# --- Prepare ---
mkdir -p $OutDir



# --- Perform Operations ---

echo
echo
echo ==================================
echo == Stabilize Fisheye Aggressive ==
echo ==================================
if [ ! -f $OutFisheyeAggressive ]
then
  $Mercalli --source $MediaGoProWide --target $OutFisheyeAggressive --RS ON --Camera "GoPro Hero3+ Wide" --TranslationSmoothing 1.0 --ZRotationSmoothing 1.0 --BorderMode Raw --stabilize
fi


echo
echo
echo ==================================
echo == Stabilize Fisheye Standard   ==
echo ==================================
if [ ! -f $OutFisheye ]
then
  $Mercalli --source $MediaGoProWide --target $OutFisheye --RS ON --Camera "GoPro Hero3+ Wide" --stabilize
fi

echo
echo
echo ====================================
echo == Stabilize Interlace Aggressive ==
echo ====================================
if [ ! -f $OutFieldsAggressive ]
then
  $Mercalli --source $MediaFields --target $OutFieldsAggressive --RS ON --TranslationSmoothing 0.95 --ZRotationSmoothing 0.95 --BorderMode Raw --stabilize
fi

echo
echo
echo ====================================
echo == Stabilize Interlace Standard   ==
echo ====================================
if [ ! -f $OutFields ]
then
  $Mercalli --source $MediaFields --target $OutFields --RS ON --stabilize
fi

echo
echo
echo ===============================
echo ==    Reflected Unjello OF   ==
echo ===============================
if [ ! -f $OutReflectedUnjelloOF ]
then
  $Mercalli --source $MediaCmosVibration --target $OutReflectedUnjelloOF --EstimateShutterSpeed ON --AutoShutterSpeed ON --unjello
fi

echo
echo
echo ===============================
echo ==    Instant Unjello OF     ==
echo ===============================
if [ ! -f $OutInstantUnjelloOF ]
then
  $Mercalli --source $MediaCmosVibration --target $OutInstantUnjelloOF --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
fi

echo
echo
echo ===============================
echo ==    Instant Unjello FP     ==
echo ===============================
if [ ! -f $OutInstantUnjelloFP ]
then
  $Mercalli --source $MediaCmosVibration --target $OutInstantUnjelloFP --UJMth P-FP --EstimateShutterSpeed OFF --AutoShutterSpeed OFF --ShutterSpeed 1.1 --unjello
fi

echo
echo
echo ===============================
echo == Stabilize Unjello-Output  ==
echo ===============================
if [ ! -f $OutVibration ]
then
  $Mercalli --source $OutReflectedUnjelloOF --target $OutVibration --Camera "GoPro Hero3+ Wide" --RS OFF --stabilize
fi



echo
echo See the windows-script for: Hyperlapse3 and Hyperlapse8
echo



exit;


echo
echo
echo .........................
echo ..... Test Finished .....
echo .........................
echo Result Videos at: $OutDir
echo
goto End

:Error
echo
echo ++++++++++++++
echo +++ failed +++
echo ++++++++++++++
echo
pause

:End

