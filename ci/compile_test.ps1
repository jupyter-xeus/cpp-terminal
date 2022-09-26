cmake -DCMAKE_INSTALL_PREFIX=./install .
cmake --build . --config Release
cmake --install . --config Release
ctest --output-on-failure
