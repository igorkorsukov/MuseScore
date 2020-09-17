echo "Start dummy build"

REM CALL "wget.exe" -q --show-progress --no-check-certificate "https://s3.amazonaws.com/utils.musescore.org/MuseScore_x86-64_msvc.install_x64.zip" -O MuseScore_x86-64_msvc.install_x64.zip
REM CALL "7z" x -y MuseScore_x86-64_msvc.install_x64.zip "-omsvc.install_x64"

REM CALL "wget.exe" -q --show-progress --no-check-certificate "https://s3.amazonaws.com/utils.musescore.org/MuseScore_x86-64_msvc.build_x64.zip" -O MuseScore_x86-64_msvc.build_x64.zip
REM CALL "7z" x -y MuseScore_x86-64_msvc.build_x64.zip "-omsvc.build_x64"

mkdir msvc.build_x64
mkdir msvc.install_x64
echo "test" > msvc.build_x64\MuseScore-3.5.0.msi
dir msvc.build_x64

echo "Finishe dummy build"
