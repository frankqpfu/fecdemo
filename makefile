all:
	g++ -c gf.cc -o gf.o
	g++ test.cc gf.o -o matrix
