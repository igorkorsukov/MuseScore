echo "Start dummy package"

SET ARTIFACTS_DIR=artifacts
MKDIR %ARTIFACTS_DIR%

SET ARTIFACT_NAME=ftp_test2.txt
ECHO "test" > %ARTIFACTS_DIR%/%ARTIFACT_NAME%
ECHO %ARTIFACT_NAME% > %ARTIFACTS_DIR%/artifact_name.env




