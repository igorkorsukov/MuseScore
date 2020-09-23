echo "Start dummy package"

SET RELEASE_CHANNEL=""
SET SIGN_CERTIFICATE_ENCRYPT_SECRET=""
SET SIGN_CERTIFICATE_PASSWORD=""

:GETOPTS
IF /I "%1" == "-c" SET RELEASE_CHANNEL=%2 & SHIFT
IF /I "%1" == "--signsecret" SET SIGN_CERTIFICATE_ENCRYPT_SECRET=%2 & SHIFT
IF /I "%1" == "--signpass" SET SIGN_CERTIFICATE_PASSWORD=%2 & SHIFT
SHIFT
IF NOT "%1" == "" GOTO GETOPTS

ECHO "SIGN_CERTIFICATE_PASSWORD: %SIGN_CERTIFICATE_PASSWORD%"



SET 

ECHO "================"
DIR "C:\Program Files (x86)"

ECHO "================"
DIR "C:\Program Files (x86)\Microsoft Visual Studio 14.0"

ECHO "================"
DIR "C:\Program Files (x86)\Microsoft Visual Studio 14.0\DIA SDK"

ECHO "================"
DIR "C:\Program Files (x86)\Microsoft Visual Studio 14.0\DIA SDK\bin"
