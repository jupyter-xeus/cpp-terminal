#! /bin/bash

set -ex

./test_terminal

./colors

echo "Expected to succeed:"
./colors < README.md
echo "Expected to fail"
set +e
./keys < README.md
if [[ $? != 0 ]]; then
  echo "Success"
else
  echo "Fail"
  exit 1
fi
set -e

echo
echo "Tests passed."
