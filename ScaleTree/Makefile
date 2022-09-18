analyze:analyze.o
	g++ analyze.o -o analyze
analyze.o:analyze.S
	g++ -c analyze.S -o analyze.o
analyze.S:analyze.i
	g++ -S analyze.i -o analyze.S
analyze.i:analyze.c
	g++ -E analyze.c -o analyze.i
