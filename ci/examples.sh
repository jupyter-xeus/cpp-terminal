#! /bin/bash

set -ex

./install/bin/examples/colors
./install/bin/examples/read_stdin

echo "testing stdin example"
if [[ $(echo test | ./install/bin/examples/read_stdin) != "Input from stdin: test" ]]; then
  echo "stdin example returned wrong input"
  exit 1
fi

echo "Expected to succeed:"
./install/bin/examples/colors < ./install/bin/examples/colors
echo "Expected to fail"
set +e
./install/bin/examples/keys < ./install/bin/examples/colors
if [[ $? != 0 ]]; then
  echo "Success"
else
  echo "Fail"
  exit 1
fi
set -e

echo
echo "Tests passed."
