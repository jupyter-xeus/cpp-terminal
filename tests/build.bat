cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=%CD%/inst .
if errorlevel 1 exit 1

cmake --build . --config Release
if errorlevel 1 exit 1

cmake --install . --config Release
if errorlevel 1 exit 1

ctest --output-on-failure
if errorlevel 1 exit 1
