ECHO "Setup Windows docker image for build MuseScore"

:: Install tools
choco install -y git.install
choco install -y wget
choco install -y 7zip.install
REM choco install -y wixtoolset

SET TEMP_DIR="c:\TEMP\musescore"
MKDIR %TEMP_DIR%

:: Install Qt
SET "Qt_ARCHIVE=Qt5151_msvc2019_64.7z"
SET "QT_URL=https://utils.musescore.org.s3.amazonaws.com/%Qt_ARCHIVE%"
SET "QT_DIR=C:\Qt\5.15.1"

CALL "wget.exe" -q --show-progress --no-check-certificate "%QT_URL%" -O "%TEMP_DIR%\%Qt_ARCHIVE%"
CALL "7z" x -y "%TEMP_DIR%\%Qt_ARCHIVE%" "-o%QT_DIR%"

SETX PATH=%QT_DIR%\msvc2019_64\bin;%PATH% /m

:: Dependency
CALL "wget.exe" -q --show-progress --no-check-certificate "http://utils.musescore.org.s3.amazonaws.com/musescore_dependencies_win32.7z" -O %TEMP_DIR%\musescore_dependencies_win32.7z
CALL "7z" x -y %TEMP_DIR%\musescore_dependencies_win32.7z "-o%TEMP_DIR%\musescore_dependencies_win32"
SET JACK_DIR="C:\Program Files (x86)\Jack"
XCOPY %TEMP_DIR%\musescore_dependencies_win32\dependencies\Jack %JACK_DIR% /E /I /Y

SETX PATH=%JACK_DIR%;%PATH% /m

CALL "wget.exe" -q --show-progress --no-check-certificate "https://s3.amazonaws.com/utils.musescore.org/dependencies.7z" -O  %TEMP_DIR%\dependencies.7z
CALL "7z" x -y %TEMP_DIR%\dependencies.7z "-oC:\musescore_dependencies"

rmdir /Q /S "C:\TEMP"

ECHO "Setup script done"