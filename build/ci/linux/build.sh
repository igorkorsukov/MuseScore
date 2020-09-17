#!/usr/bin/env bash

echo "Build Linux MuseScore AppImage"

# Go one-up from MuseScore root dir regardless of where script was run from:
cd "$(dirname "$(readlink -f "${0}")")/../../../.."

##########################################################################
# SETUP ENVIRONMENT
##########################################################################
echo "=== ENVIRONMENT === "

TELEMETRY_TRACK_ID=$1

ENV_FILE=./musescore_environment.sh
cat ${ENV_FILE}
. ${ENV_FILE}

echo "===================="
ls -all /home/runner/work/MuseScore/appimagetool
whereis appimagetool
echo "===================="

echo " "
${CXX} --version 
${CC} --version
echo " "
cmake --version
echo " "
appimagetool --version
echo " "
linuxdeploy --list-plugins
echo "===================="
echo " "
env
echo " "


##########################################################################
# BUILD MUSESCORE
##########################################################################

cd MuseScore
#rm -rf ./build.*
make revision
make "TELEMETRY_TRACK_ID=${TELEMETRY_TRACK_ID}" portable
cd ..

