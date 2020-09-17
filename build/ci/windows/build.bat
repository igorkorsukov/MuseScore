

XCOPY "C:\musescore_dependencies" %CD% /E /I /Y

SET TELEMETRY_TRACK_ID=%1
SET CRASH_LOG_SERVER_URL=%2

SET BUILD_NUMBER=42
SET TARGET_PROCESSOR_BITS=64
SET GENERATOR_NAME=Visual Studio 16 2019
SET MSCORE_STABLE_BUILD="TRUE"

:: TODO We need define paths during image creation
SET "QT_DIR=C:\Qt\5.9.9"
SET "JACK_DIR=C:\Program Files (x86)\Jack"
SET PATH=%QT_DIR%\msvc2017_64\bin;%JACK_DIR%;%PATH% 

CALL msvc_build.bat revision 
CALL msvc_build.bat relwithdebinfo %TARGET_PROCESSOR_BITS% %BUILD_NUMBER%
CALL msvc_build.bat installrelwithdebinfo

