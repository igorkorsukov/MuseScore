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
<<<<<<< HEAD

=======
    
>>>>>>> 344f6fbb3c116446ea2a5b2b90943007eecfd069
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

<<<<<<< HEAD
SET SSH_IDENTITY=..\build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp
icacls %SSH_IDENTITY% /inheritance:r
icacls %SSH_IDENTITY%y /grant:r "%username%":"(R)"

=======
>>>>>>> 344f6fbb3c116446ea2a5b2b90943007eecfd069
IF NOT EXIST %ARTIFACT_NAME% (
    ECHO "Not exists artifact, name: %ARTIFACT_NAME%"
    EXIT 1
)
<<<<<<< HEAD
=======

SET SSH_KEY=..\build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp

Icacls %SSH_KEY%
Icacls %SSH_KEY% /Inheritance:r
Icacls %SSH_KEY% /Grant:r "%Username%":(R,W)
Icacls %SSH_KEY%
>>>>>>> 344f6fbb3c116446ea2a5b2b90943007eecfd069

scp -oStrictHostKeyChecking=no -C -i %SSH_KEY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/

CD ..