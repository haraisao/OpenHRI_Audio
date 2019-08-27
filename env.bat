
@if "%1" == "" @(
    @set DRV=E
) else (
    @set DRV=%1
)

@call %DRV%:\local\OpenRTM-aist\setup.bat
@set PATH=%~dp0bin;%PATH%