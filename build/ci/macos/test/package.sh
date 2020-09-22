BUILD_DIR="build.app"
mkdir ${BUILD_DIR}

echo "test" > ${BUILD_DIR}/MuseScore-3.5.42.dmg

DMGFILE="$(ls ${BUILD_DIR}/*.dmg)"

ARTIFACT_NAME="$(basename "${DMGFILE}")"
echo "ARTIFACT_NAME: ${ARTIFACT_NAME}"

ARTIFACTS_DIR="build.artifacts"
mkdir ${ARTIFACTS_DIR}

mv ${DMGFILE} ${ARTIFACTS_DIR}/${ARTIFACT_NAME}"_test"
echo ${ARTIFACT_NAME}"_test" > ${ARTIFACTS_DIR}/artifact_name.env

echo "--ls ARTIFACTS_DIR:"
ls ${ARTIFACTS_DIR}

echo "--cat ${ARTIFACTS_DIR}/artifact_name.env:"
cat ${ARTIFACTS_DIR}/artifact_name.env