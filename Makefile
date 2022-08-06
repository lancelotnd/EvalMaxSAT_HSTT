all:
	g++ -std=c++2a lib/pugixml-1.12/src/pugixml.cpp main.cpp libipamir_O0.a -o main -lz -pthread -g -O0
