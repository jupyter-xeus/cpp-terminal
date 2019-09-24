all: kilo menu keys colors

kilo: examples/kilo.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/kilo.cpp -o kilo -Wall -Wextra --std=c++14

menu: examples/menu.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/menu.cpp -o menu -Wall -Wextra --std=c++14

keys: examples/keys.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/keys.cpp -o keys -Wall -Wextra --std=c++14

colors: examples/colors.cpp terminal.h terminal_base.h
	$(CXX) -I. examples/colors.cpp -o colors -Wall -Wextra --std=c++14
