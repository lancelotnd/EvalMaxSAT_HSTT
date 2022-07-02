all:
	g++ main.cpp libEvalMaxSAT_bin.a -o main -lz -pthread -g