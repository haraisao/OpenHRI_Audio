set INSTALL_DIR=%~d0\local\OpenHRIAudio

xcopy /E /Y %~dp0Components %INSTALL_DIR%\Components\
xcopy /E /Y %~dp0bin %INSTALL_DIR%\bin\
xcopy  /Y %~dp0LICENSE %INSTALL_DIR%\
xcopy  /Y %~dp0README.md %INSTALL_DIR%\
xcopy  /Y %~dp0package.xml %INSTALL_DIR%\
xcopy  /Y %~dp0env.bat %INSTALL_DIR%\

cd /d %INSTALL_DIR%\..
tar czvf %~dp0package\OpenHRIAudio.tgz OpenHRIAudio

cd %~dp0