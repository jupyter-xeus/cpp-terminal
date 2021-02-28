#!/bin/bash

set -ex

cmake -DCMAKE_INSTALL_PREFIX=./inst .
cmake --build . --config Release
cmake --install . --config Release
ctest --output-on-failure

cd tests/test-standalone
cmake -DCMAKE_PREFIX_PATH=../inst .
cmake --build . --config Release
