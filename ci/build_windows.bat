REM Check versions
qmake -v
cmake --version
ninja --version
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

mkdir build
cd build

call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake .. -GNinja -DCMAKE_PREFIX_PATH="%QT_ROOT_DIR%;%CONDA_PREFIX%\Library" -DCMAKE_BUILD_TYPE=Release

IF %ERRORLEVEL% NEQ 0 GOTO ERROR

cmake --build . --config Release

IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO END

:ERROR

echo "Building e57inspector failed"
exit /b 1

:END