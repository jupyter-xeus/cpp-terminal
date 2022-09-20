#!/bin/bash

set -ex

cmake -DCMAKE_INSTALL_PREFIX=./install .
cmake --build . --config Release
cmake --install . --config Release
ctest --output-on-failure

cd tests/test-standalone
cmake -DCMAKE_PREFIX_PATH=../../install .
cmake --build . --config Release
