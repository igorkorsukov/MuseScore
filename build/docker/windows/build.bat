XCOPY "C:\musescore_dependencies" "c:\MuseScore" /E /I /Y

SET GENERATOR_NAME=Visual Studio 16 2019
SET MSCORE_STABLE_BUILD="TRUE"

:: TODO We need define paths during image creation
SET "QT_DIR=C:\Qt\5.15.1"
SET "JACK_DIR=C:\Program Files (x86)\Jack"
SET PATH=%QT_DIR%\msvc2019_64\bin;%JACK_DIR%
;%PATH% 


CD C:\MuseScore
CALL msvc_build.bat revision 
SET /p MSREVISION=<mscore\revision.h

CALL msvc_build.bat relwithdebinfo 
CALL msvc_build.bat installrelwithdebinfo

ECHO "7z package"
CD C:\MuseScore
COPY C:\MuseScore\mscore\revision.h C:\MuseScore\msvc.install_x64\revision.h

7z a C:\MuseScore\MuseScore_x86-64.7z C:\MuseScore\msvc.install_x64