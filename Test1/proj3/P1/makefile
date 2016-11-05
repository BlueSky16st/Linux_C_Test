.PHONY:clean all

all:libaddsub.so

libaddsub.so:add.cpp sub.cpp
	g++ -g -Wall -shared -fPIC $^ -o $@

clean:
	rm -rf libaddsub.so

