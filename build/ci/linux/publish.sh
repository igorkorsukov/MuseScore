#!/usr/bin/env bash

echo "Publish MuseScore"

ARTIFACTS_DIR=build.artifacts

OSUOSL_SSH_ENCRYPT_SECRET=$1
ARTIFACT_NAME=$2

if [ -z "$1" ]
  then
    echo "Not set ssh encrypt secret"
    exit 1
fi

if [ -z "$2" ]
  then
    echo "Try get artifact name from ${ARTIFACTS_DIR}/artifact_name.env"
    ARTIFACT_NAME="$(cat ${ARTIFACTS_DIR}/artifact_name.env)"   
fi

if [ -z "${ARTIFACT_NAME}" ]
  then
    echo "Not set artifact name"
    exit 1
fi

echo "[publish] ARTIFACT_NAME: ${ARTIFACT_NAME}"

7z e build/ci/tools/osuosl/osuosl_nighlies_rsa.enc -p${OSUOSL_SSH_ENCRYPT_SECRET}

SSH_IDENTITY=build/ci/tools/osuosl/osuosl_nighlies_rsa

scp -oStrictHostKeyChecking=no -C -i ${SSH_IDENTITY} ${ARTIFACT_NAME} musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/


