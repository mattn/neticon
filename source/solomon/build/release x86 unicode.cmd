@REM
@REM build
@REM

@REM
@REM 準備
@REM

@SET BUILD_NAME=%~n0
@SET BUILD_BODY_CMD=%~dp0subcmd\build.cmd

@REM
@REM 本体処理呼び出し
@REM

@CALL "%BUILD_BODY_CMD%" %~n0
