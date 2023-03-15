echo "running example programs"
.\install\bin\examples\colors.exe

echo "testing read_stind"
if ((echo test | .\install\bin\examples\read_stdin.exe) -ne "Input from stdin: test")
{
    echo "read_stdin gave wrong output"
    exit 1
}
