all: test_terminal prompt kilo menu menu_window keys colors

test_terminal: test_terminal.cpp terminal.h terminal_base.h
	$(CXX) -g -I. test_terminal.cpp -o test_terminal -Wall -Wextra --std=c++11

prompt: examples/prompt.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/prompt.cpp -o prompt -Wall -Wextra --std=c++11

kilo: examples/kilo.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/kilo.cpp -o kilo -Wall -Wextra --std=c++11

menu: examples/menu.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/menu.cpp -o menu -Wall -Wextra --std=c++11

menu_window: examples/menu_window.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/menu_window.cpp -o menu_window -Wall -Wextra --std=c++11

keys: examples/keys.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/keys.cpp -o keys -Wall -Wextra --std=c++11

colors: examples/colors.cpp terminal.h terminal_base.h
	$(CXX) -g -I. examples/colors.cpp -o colors -Wall -Wextra --std=c++11
