CPP := g++
SOURCES := *.cpp
FLAGS := -ldl -g
.PHONY: all clean install uninstall

all:
	$(CPP) $(SOURCES) -o yoda $(FLAGS)

clean:
	rm yoda

install: all
	mv yoda /usr/bin/yoda

uninstall:
	rm /usr/bin/yoda
