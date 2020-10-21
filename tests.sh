#! /bin/bash

set -ex

./test_terminal

examples/colors

echo "Expected to succeed:"
examples/colors < README.md
echo "Expected to succeed:"
set +e
examples/keys < README.md

uname=`uname`

if [[ $? == 0 -and $uname == "Linux" ]]; then
  echo "Success"
else
  echo "Fail"
  exit 1
fi
set -e

echo
echo "Tests passed."
