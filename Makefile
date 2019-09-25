all: prompt kilo menu keys colors

prompt: examples/prompt.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/prompt.cpp -o prompt -Wall -Wextra --std=c++11

kilo: examples/kilo.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/kilo.cpp -o kilo -Wall -Wextra --std=c++11

menu: examples/menu.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/menu.cpp -o menu -Wall -Wextra --std=c++11

keys: examples/keys.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/keys.cpp -o keys -Wall -Wextra --std=c++11

colors: examples/colors.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/colors.cpp -o colors -Wall -Wextra --std=c++11
