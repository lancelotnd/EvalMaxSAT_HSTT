all:
	g++ pugixml-1.12/src/pugixml.cpp main.cpp libEvalMaxSAT_bin.a -o main -lz -pthread -g
