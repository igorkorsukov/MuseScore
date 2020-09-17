
:: Common
SET BUILD_CONFIG=%1
SET BUILD_DIR=msvc.build_x64
SET INSTALL_DIR=msvc.install_x64
SET ARTEFACTS_DIR=artefacts
SET TARGET_PROCESSOR_BITS=64
SET TARGET_PROCESSOR_ARCH=x86_64

:: For MSI
SET SIGN_CERTIFICATE_ENCRYPT_SECRET=%2
SET SIGN_CERTIFICATE_PASSWORD=%3
SET SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"
SET UUIDGEN="C:\Program Files (x86)\Windows Kits\10\bin\x64\uuidgen.exe"
SET WIX_DIR=%WIX%

:: Setup package type
IF %BUILD_CONFIG% == stable ( 
    SET PACKAGE_TYPE="msi" 
) ELSE (
IF %BUILD_CONFIG% == testing ( 
    SET PACKAGE_TYPE="msi" 
) ELSE (
IF %BUILD_CONFIG% == dev ( 
    SET PACKAGE_TYPE="7z" 
) ELSE (
    ECHO "Unknown BUILD_CONFIG: %BUILD_CONFIG%"
    goto END_ERROR
)
)
)

ECHO "PACKAGE_TYPE: %PACKAGE_TYPE%"
IF %PACKAGE_TYPE% == "7z" (
    goto PACK_7z
) ELSE (
IF %PACKAGE_TYPE% == "msi" (
    goto PACK_MSI
) ELSE (
    ECHO "Unknown package type: %PACKAGE_TYPE%"
    goto END_ERROR
)
)


:PACK_7z
ECHO "Start 7z packing..."
COPY mscore\revision.h %INSTALL_DIR%\revision.h
7z a MuseScore_x86-64.7z %INSTALL_DIR%
ECHO "Finished 7z packing"
goto END_SUCCESS

:PACK_MSI

:: sign dlls and exe files
choco install -y --ignore-checksums secure-file
secure-file -decrypt build\ci\windows\resources\musescore.p12.enc -secret %SIGN_CERTIFICATE_ENCRYPT_SECRET%

for /f "delims=" %%f in ('dir /a-d /b /s "%INSTALL_DIR%\*.dll" "%INSTALL_DIR%\*.exe"') do (
    ECHO "Signing %%f"
    %SIGNTOOL% sign /f "build\ci\windows\resources\musescore.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll /p "%SIGN_CERTIFICATE_PASSWORD%" "%%f"
)

:: generate unique GUID
%UUIDGEN% > uuid.txt
SET /p PACKAGE_UUID=<uuid.txt
ECHO on
ECHO "PACKAGE_UUID: %PACKAGE_UUID%"
ECHO off
sed -i 's/00000000-0000-0000-0000-000000000000/%PACKAGE_UUID%/' build/Packaging.cmake

SET PATH=%WIX_DIR%;%PATH% 
CALL msvc_build.bat package %TARGET_PROCESSOR_BITS%

ECHO "--------------"

:: find the MSI file without the hardcoded version
for /r %%i in (%BUILD_DIR%\*.msi) do (
  SET "FILEPATH=%%i"
  SET "FILEBASE=%%~ni"
  SET "FILEEXT=%%~xi"
  SET "FILEDIR=%%~dpi"
  )
ECHO "Package: %FILEPATH%"
SET "FILEBASE=%FILEBASE%-%TARGET_PROCESSOR_ARCH%"
SET "FILENAME=%FILEBASE%%FILEEXT%"
RENAME "%FILEPATH%" "%FILENAME%"
SET "FILEPATH=%FILEDIR%%FILENAME%"
ECHO "Renamed: %FILENAME%"
ECHO "Location: %FILEPATH%"
@ECHO off
%SIGNTOOL% sign /debug /f "build\ci\windows\resources\musescore.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll /p "%SIGN_CERTIFICATE_PASSWORD%" /d %FILENAME% %FILEPATH%
:: verify signature
%SIGNTOOL% verify /pa %FILEPATH%

:: prepare upload
MKDIR %ARTEFACTS_DIR%
XCOPY %FILEPATH% %ARTEFACTS_DIR% /Y /Q
SET ARTIFACT_NAME=%FILENAME%

REN @echo off
REM WinSparkle staff. Generate appcast.xml
REM ------------------------------------------
REM bash build\appveyor\winsparkle_appcast_generator.sh "%ARTEFACTS_DIR%\%ARTIFACT_NAME%" "https://ftp.osuosl.org/pub/musescore-nightlies/windows/%ARTIFACT_NAME%" "%MUSESCORE_VERSION%" "%MSREVISION%"
REM ------------------------------------------
REN @echo on
REN type C:\MuseScore\appcast.xml

REM SET /p MSCORE_RELEASE_CHANNEL=<MSCORE_RELEASE_CHANNEL.xml

ECHO "Finished msi packing"

:END_SUCCESS
exit 0

:END_ERROR
exit 1