ECHO "Publish package"

SET ARTIFACTS_DIR=build.artifacts

SET OSUOSL_SSH_ENCRYPT_SECRET=%1
SET ARTIFACT_NAME=%2

IF [%1]==[] (
    ECHO "Not set ssh encrypt secret"
    EXIT 1
)

IF [%2]==[] (
    ECHO "Try get artifact name from %ARTIFACTS_DIR%/artifact_name.env"
    SET /p ARTIFACT_NAME=<%ARTIFACTS_DIR%/artifact_name.env
)

IF %ARTIFACT_NAME% == "" (
    ECHO "Not set artifact name"
    EXIT 1
)

ECHO "[publish] ARTIFACT_NAME: %ARTIFACT_NAME%"

REM where /q secure-file
REM IF ERRORLEVEL 1 ( choco install -y choco install -y --ignore-checksums secure-file )
REM secure-file -decrypt build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp.enc -secret %OSUOSL_SSH_ENCRYPT_SECRET%

7z e build\ci\tools\osuosl\osuosl_nighlies_rsa.enc -p$%OSUOSL_SSH_ENCRYPT_SECRET%

SET SSH_IDENTITY=build\ci\tools\osuosl\osuosl_nighlies_rsa

scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/

