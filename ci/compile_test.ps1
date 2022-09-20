cmake -DCMAKE_INSTALL_PREFIX=./install .
cmake --build .
cmake --install .
ctest --output-on-failure
