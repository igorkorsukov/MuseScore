ECHO "Publish package"

SET ARTIFACTS_DIR=artifacts

SET OSUOSL_SSH_ENCRYPT_SECRET=%1
SET ARTIFACT_PATH=%2

IF [%1]==[] (
    ECHO "Not set ssh encrypt secret"
    EXIT 1
)

IF [%2]==[] (
    ECHO "Try get artifact name from %ARTIFACTS_DIR%\artifact_name.env"

    type %ARTIFACTS_DIR%\artifact_name.env

    SET /p ARTIFACT_NAME=<%ARTIFACTS_DIR%\artifact_name.env
)

IF "%ARTIFACT_NAME%" == "" (
    ECHO "Not set artifact name"
    EXIT 1
)

ECHO "[publish] ARTIFACT_PATH: %ARTIFACT_PATH%"

IF NOT EXIST %ARTIFACT_PATH% (
    ECHO "Not exists artifact, path: %ARTIFACT_PATH%"
    EXIT 1
)

where /q secure-file
IF ERRORLEVEL 1 ( choco install -y choco install -y --ignore-checksums secure-file )
secure-file -decrypt build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp.enc -secret %OSUOSL_SSH_ENCRYPT_SECRET%

CD %ARTIFACTS_DIR%

SET SSH_IDENTITY=..\build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp
icacls %SSH_IDENTITY% /inheritance:r
icacls %SSH_IDENTITY%y /grant:r "%username%":"(R)"

IF NOT EXIST %ARTIFACT_NAME% (
    ECHO "Not exists artifact, name: %ARTIFACT_NAME%"
    EXIT 1
)

scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/

CD ..