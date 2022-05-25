cmake -DCMAKE_INSTALL_PREFIX=%cd%\inst .
cmake --build . --config Release
cmake --install . --config Release
ctest --output-on-failure

cd tests\test-standalone
cmake -DCMAKE_PREFIX_PATH=%cd%\..\inst .
cmake --build . --config Release
cd
