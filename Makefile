CPP := g++
SOURCES := \
	main.cpp \
	string_stack.cpp \
	terminal_colors.cpp \
	user_variables.cpp

.PHONY: all clean install uninstall

all:
	$(CPP) $(SOURCES) -o yoda

clean:
	rm yoda

install: all
	mv yoda /usr/bin/yoda

uninstall:
	rm /usr/bin/yoda
