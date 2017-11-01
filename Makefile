CPP := g++
SOURCES := *.cpp

.PHONY: all clean install uninstall

all:
	$(CPP) $(SOURCES) -o yoda -g

clean:
	rm yoda

install: all
	mv yoda /usr/bin/yoda

uninstall:
	rm /usr/bin/yoda
