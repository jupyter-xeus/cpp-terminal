echo "running example programs"
.\examples\Release\colors.exe
.\tests\Release\test_terminal.exe
.\examples\Release\read_stdin.exe

echo "testing read_stind"
if ((echo test | .\examples\Release\read_stdin.exe) -ne "Input from stdin: test") {
    echo "read_stdin gave wrong output"
    exit 1
}
