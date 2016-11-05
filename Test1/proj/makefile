.PHONY:clean
File = start
SourceFile = $(wildcard *.cpp)
FileObj = $(SourceFile:%.cpp=%.o)
CPP = g++
CPPFlags = -g -Wall

$(File):$(FileObj)
	$(CPP) $(CPPFlags) $^ -o $@ -lpthread
	rm -rf $(FileObj)

%.o:%.cpp
	$(CPP) $(CPPFlags) -c $< -o $@

start.o:start.cpp
	$(CPP) $(CPPFlags) -c $< -o $@ -I../include/

A.o:A.cpp
	$(CPP) $(CPPFlags) -c $< -o $@

#start.o:start.cpp
#	$(CPP) $(CPPFlags) -c $< -o $@ -I../include/

#calc.o:calc.cpp
#	$(CPP) $(CPPFlags) -c $< -o $@

#A.o:A.cpp
#	$(CPP) $(CPPFlags) -c $< -o $@

#B.o:B.cpp
#	$(CPP) $(CPPFlags) -c $< -o $@

#C.o:C.cpp
#	$(CPP) $(CPPFlags) -c $< -o $@

clean:
	rm -rf $(FileObj)

print:
	@echo $(FileObj)
