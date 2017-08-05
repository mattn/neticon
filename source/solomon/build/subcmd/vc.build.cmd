@REM
@REM build
@REM


@REM
@REM コンパイル
@REM

@REM コンパイルログ出力先
@SET RC_COMPILE_LOG_FILE=.\rc_compile.log
@SET COMPILE_LOG_FILE=.\compile.log

@REM コンパイル結果のコピー先ディレクトリの作成
@SET RESULT_DIR=%SOLOMON_SNAPSHOT_DIR%\result\%BUILD_NAME%
@IF NOT EXIST "%RESULT_DIR%" CALL "%SOLOMON_LIBCMD_DIR%\mkdir.ex.cmd" "%RESULT_DIR%"

@REM コンパイル結果ディレクトリにバージョン情報ファイルが存在していたら削除
@IF EXIST "%RESULT_DIR%\VERSION.cmd" DEL "%RESULT_DIR%\VERSION.cmd"

@REM リソースコンパイル
@SETLOCAL
@CALL "%CALL_VCVARSALL_CMD%" %VCVARSALL_ARG% >NUL
@SET COMPILE_EXECUTE_CMD=rc %NETICON_MACRO% .\neticon.rc
@ECHO ⇒%COMPILE_EXECUTE_CMD%
@%COMPILE_EXECUTE_CMD%>>"%RC_COMPILE_LOG_FILE%" 2>&1
@ENDLOCAL

@REM リソースコンパイル結果判定
@IF NOT EXIST ".\neticon.rc" CALL "%SOLOMON_COMPILE_FAILED_CMD%" %NETICON_MACRO% "%RC_COMPILE_LOG_FILE%" & GOTO :EOF

@REM リソースコンパイル警告判定
@TYPE "%RC_COMPILE_LOG_FILE%" | FIND /I " warning " >NUL
@IF /I "1" NEQ "%ERRORLEVEL%" CALL "%SOLOMON_COMPILE_WARNED_CMD%" "%RC_COMPILE_LOG_FILE%"

@REM 通常コンパイル
@SETLOCAL
@CALL "%CALL_VCVARSALL_CMD%" %VCVARSALL_ARG% >NUL
@SET COMPILE_EXECUTE_CMD=cl ".\neticon.cpp" %VC_CL_ARG% %NETICON_MACRO% /EHsc /MP /W4 /Feneticon.exe /link %VCLINKER_ARG% neticon.res
@ECHO ⇒%COMPILE_EXECUTE_CMD%
@%COMPILE_EXECUTE_CMD%>>"%COMPILE_LOG_FILE%" 2>&1
@ENDLOCAL

@REM 通常コンパイル結果判定
@IF NOT EXIST ".\neticon.exe" CALL "%SOLOMON_COMPILE_FAILED_CMD%" "%COMPILE_LOG_FILE%" & GOTO :EOF

@REM 通常コンパイル警告判定
@TYPE "%COMPILE_LOG_FILE%" | FIND /I " warning " >NUL
@IF /I "1" NEQ "%ERRORLEVEL%" CALL "%SOLOMON_COMPILE_WARNED_CMD%" "%COMPILE_LOG_FILE%"

@REM コンパイル結果をコピー
@COPY /Y ".\*.exe" "%RESULT_DIR%" >NUL
@COPY /Y ".\VERSION.cmd" "%RESULT_DIR%" >NUL

@REM コンパイルが成功したことの通知
CALL "%SOLOMON_COMPILE_SUCCESS_CMD%"
