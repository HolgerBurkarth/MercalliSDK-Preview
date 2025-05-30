#!/bin/bash
#
# $CRT 28 Mai 2025 : hb
#
# $AUT Holger Burkarth
# $DAT >>LinuxValidateMercalliCli.sh<< 28 Mai 2025  11:07:23 - (c) proDAD

# --- Setup ---
SDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MercalliDir=$SDir/../bin/linux/Release/
Mercalli=$MercalliDir/mercalli_static.bin
MediaDir=$SDir/media/
OutDir=$SDir/out/

MediaGoProWide=$MediaDir/CmosGoPro3Wide

OutFisheyeForensic=$OutDir/CmosGoPro3Wide


# --- Prepare ---
mkdir -p $OutDir



# --- Perform Operations ---

echo
echo
echo ==================================
echo == Stabilize Fisheye Forensic   ==
echo ==================================
if [ ! -f $OutFisheyeForensic ]
then
  $Mercalli --source $MediaGoProWide --target $OutFisheyeForensic --RS ON --Camera "GoPro Hero3+ Wide" --TranslationSmoothing 1.0 --ZRotationSmoothing 1.0 --BorderMode Forensic --stabilize
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

