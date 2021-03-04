#! /bin/bash

set -ex

#cd ..

tests/test_terminal

examples/colors

echo "Expected to succeed:"
examples/colors < README.md
echo "Expected to fail"
set +e
examples/keys < README.md
if [[ $? != 0 ]]; then
  echo "Success"
else
  echo "Fail"
  exit 1
fi
set -e

echo
echo "Tests passed."
