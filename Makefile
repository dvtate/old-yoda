CPP := g++
SOURCES := *.cpp
FLAGS := -g
.PHONY: all libs clean install uninstall full

# compile yodascript binary
all:
	$(CPP) $(SOURCES) -o yoda -ldl $(FLAGS)

# compile language extension support
libs:
	$(CPP) user_defs.cpp -fPIC -shared $(FLAGS) -o yslib.so
	
# clear previous builds
clean:
	rm yoda
	rm yslib.so



install: all libs
	mv yoda /usr/bin/yoda
	mv yslib.so /usr/lib/yslib.so

uninstall:
	rm /usr/bin/yoda
	rm /usr/lib/yslib.so
