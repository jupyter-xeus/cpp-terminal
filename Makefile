all: test_terminal prompt kilo menu menu_window keys colors

CXXFLAGS=-g -I. -Wall -Wextra --std=c++11

test_terminal: test_terminal.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) test_terminal.cpp -o test_terminal

prompt: examples/prompt.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/prompt.cpp -o prompt

kilo: examples/kilo.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/kilo.cpp -o kilo

menu: examples/menu.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/menu.cpp -o menu

menu_window: examples/menu_window.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/menu_window.cpp -o menu_window

keys: examples/keys.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/keys.cpp -o keys

colors: examples/colors.cpp terminal.h terminal_base.h
	$(CXX) $(CXXFLAGS) examples/colors.cpp -o colors
