echo "running example programs"
examples\Release\colors.exe
tests\test_terminal.exe
examples\read_stdin.exe

echo "testing read_stind"
if ("test" examples\read_stdin -ne "Input from stdin: test") {
    echo "read_stdin gave wrong output"
    exit 1
}
