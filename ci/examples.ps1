echo "running example programs"
.\examples\install\bin\colors.exe
.\examples\install\bin\read_stdin.exe

echo "testing read_stind"
if ((echo test | .\examples\install\bin\read_stdin.exe) -ne "Input from stdin: test") {
    echo "read_stdin gave wrong output"
    exit 1
}
