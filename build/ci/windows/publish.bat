ECHO "Publish package"

SET ARTIFACT_NAME=%1

where /q secure-file
IF ERRORLEVEL 1 ( choco install -y choco install -y --ignore-checksums secure-file )
secure-file -decrypt build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp.enc -secret %OSUOSL_SSH_ENCRYPT_SECRET%

SET SSH_IDENTITY=build\ci\tools\osuosl\osuosl_nighlies_rsa_nopp
REM SET PATH=%OLD_PATH%
REM REM Remove OpenSSH from PATH, to force the use of msys64 ssh
REM SET PATH=%PATH:C:\Windows\System32\OpenSSH\;=%

scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% %ARTIFACT_NAME% musescore-nightlies@ftp-osl.osuosl.org:~/ftp/windows/
ssh -oStrictHostKeyChecking=no -i %SSH_IDENTITY% musescore-nightlies@ftp-osl.osuosl.org "cd ~/ftp/windows; ls MuseScoreNightly* -t | tail -n +41 | xargs rm -f"
:: Create and upload index.html and RSS
python build/ci/tools/osuosl/win_updateHTML.py %SSH_IDENTITY%
scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% build/ci/tools/osuosl/web/index.html musescore-nightlies@ftp-osl.osuosl.org:ftp/windows
scp -oStrictHostKeyChecking=no -C -i %SSH_IDENTITY% build/ci/tools/osuosl/web/nightly.xml musescore-nightlies@ftp-osl.osuosl.org:ftp/windows
:: Trigger distribution
ssh -oStrictHostKeyChecking=no -i %SSH_IDENTITY% musescore-nightlies@ftp-osl.osuosl.org "~/trigger-musescore-nightlies"
