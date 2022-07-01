@echo off

:: D3DX
"%~dp0nuget.exe" install Microsoft.DXSDK.D3DX -Version 9.29.952.8 -OutputDirectory "%~dp0d3dx"

:: FBX
powershell Invoke-WebRequest https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/2020-2/fbx20202_fbxsdk_vs2019_win.exe -O "%~dp0fbx20202_fbxsdk_vs2019_win.exe"
"%~dp07z.exe" x "%~dp0fbx20202_fbxsdk_vs2019_win.exe" -y -o"%~dp0fbx"