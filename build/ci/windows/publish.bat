ECHO "Publish package"

SET ARTIFACTS_DIR=build.artifacts

SET OSUOSL_SSH_ENCRYPT_SECRET=%1
SET ARTIFACT_NAME=%2

IF [%1]==[] (
    ECHO "Not set ssh encrypt secret"
    EXIT 1
)

IF [%2]==[] (
    ECHO "Try get artifact name from %ARTIFACTS_DIR%\artifact_name.env"
    SET /p ARTIFACT_NAME=<%ARTIFACTS_DIR%\artifact_name.env
)

IF %ARTIFACT_NAME% == "" (
    ECHO "Not set artifact name"
    EXIT 1
)

ECHO "[publish] ARTIFACT_PATH: %ARTIFACT_PATH%"

IF NOT EXIST %ARTIFACT_PATH% (
    ECHO "Not exists artifact, path: %ARTIFACT_PATH%"
    EXIT 1
)

7z e build\ci\tools\osuosl\osuosl_nighlies_rsa.enc -p$%OSUOSL_SSH_ENCRYPT_SECRET%

CD %ARTIFACTS_DIR%

IF NOT EXIST %ARTIFACT_NAME% (
    ECHO "Not exists artifact, name: %ARTIFACT_NAME%"
    EXIT 1
)

SET SSH_KEY=..\build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp

Icacls %SSH_KEY%
Icacls %SSH_KEY% /Inheritance:r
Icacls %SSH_KEY% /Grant:r "%Username%":(R,W)
Icacls %SSH_KEY%

scp -oStrictHostKeyChecking=no -C -i %SSH_KEY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/

CD ..