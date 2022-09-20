#cmake -DCMAKE_INSTALL_PREFIX=.\inst .
cmake .
cmake --build . --config Release
cmake --install ../binaries --config Release
ctest --output-on-failure

# Install on windows is complicated and not like on linux
#cd tests\test-standalone
#cmake -DCMAKE_PREFIX_PATH=%cd%\..\inst .
#cmake --build . --config Release
#cd
