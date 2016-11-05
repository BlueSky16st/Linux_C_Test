.PHONY:clean all

all:libmuldiv.so

libmuldiv.so:mul.cpp div.cpp
	g++ -g -Wall -shared -fPIC $^ -o $@

clean:
	rm -rf libmuldiv.so

