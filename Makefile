CPP := g++
SOURCES := *.cpp
CPP_FLAGS := -lpthread

.PHONY: all clean install uninstall

all:
	$(CPP) $(SOURCES) -o yoda -g $(CPP_FLAGS)

clean:
	rm yoda

install: all
	mv yoda /usr/bin/yoda

uninstall:
	rm /usr/bin/yoda
