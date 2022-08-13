all:
	#g++ -std=c++2a lib/pugixml-1.12/src/pugixml.cpp main.cpp libipamir_O0.a -o main -lz -g -pthread  -O0
	g++ -std=c++2a lib/pugixml-1.12/src/pugixml.cpp main.cpp libipamirEvalMaxSAT2022.a -o main -lz -pthread  -O3
debug:
	 g++ -std=c++2a lib/pugixml-1.12/src/pugixml.cpp main.cpp libEvalMaxSAT_binO0.a -o main -lz -g -pthread  -O0