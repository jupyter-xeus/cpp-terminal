call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
cl -I. /EHsc examples/kilo.cpp /Fe:kilo.exe
cl -I. /EHsc examples/menu.cpp /Fe:menu.exe
cl -I. /EHsc examples/keys.cpp /Fe:keys.exe
cl -I. /EHsc examples/colors.cpp /Fe:colors.exe
