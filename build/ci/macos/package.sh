#!/usr/bin/env bash

echo "Package MuseScore"

mkdir -p applebuild/mscore.app/Contents/Resources/Frameworks
wget -c --no-check-certificate -nv -O musescore_dependencies_macos.zip  http://utils.musescore.org.s3.amazonaws.com/musescore_dependencies_macos.zip
unzip musescore_dependencies_macos.zip -d applebuild/mscore.app/Contents/Resources/Frameworks

build/package_mac

DMGFILE="$(ls applebuild/*.dmg)"

echo "DMGFILE: ${DMGFILE}"

ARTIFACTS_DIR="build.artifacts"
mkdir ${ARTIFACTS_DIR}

ARTIFACT_NAME="$(basename "${DMGFILE}")"

mv ${DMGFILE} ${ARTIFACTS_DIR}/${ARTIFACT_NAME}"_test"

echo ${ARTIFACT_NAME}"_test" > ${ARTIFACTS_DIR}/artifact_name.env

echo "--ls ARTIFACTS_DIR:"
ls ${ARTIFACTS_DIR}

echo "--cat ${ARTIFACTS_DIR}/artifact_name.env:"
cat ${ARTIFACTS_DIR}/artifact_name.env
