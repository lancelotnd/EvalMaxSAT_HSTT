all:
	g++ lib/pugixml-1.12/src/pugixml.cpp main.cpp libipamirEvalMaxSAT2022.a -o main -lz -pthread -O3
