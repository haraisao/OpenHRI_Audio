
@if "%1" == "" @(
    @set DRV=%~d0
) else (
    @set DRV=%1
)

@call %DRV%\local\OpenRTM-aist\setup.bat
@set PATH=%~dp0bin;%PATH%