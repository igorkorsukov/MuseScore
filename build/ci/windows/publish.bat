ECHO "Publish package"

IF [%1]==[] (
    ECHO "Not set artifact name"
    EXIT 1
)

IF [%2]==[] (
    ECHO "Not set ssh encrypt secret"
    EXIT 1
)


SET ARTIFACT_NAME=%1
SET OSUOSL_SSH_ENCRYPT_SECRET=%2

ECHO "[publish] ARTIFACT_NAME: %ARTIFACT_NAME%"

where /q secure-file
IF ERRORLEVEL 1 ( choco install -y choco install -y --ignore-checksums secure-file )
secure-file -decrypt build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp.enc -secret %OSUOSL_SSH_ENCRYPT_SECRET%

SET SSH_IDENTITY=build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp

scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/

