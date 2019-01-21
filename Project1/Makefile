all: phistogram thistogram;

phistogram: phistogram.c
	gcc -Wall -o phistogram phistogram.c -lrt
	#./phistogram 0 10 10 3 input1.txt input2.txt input3.txt presults.txt

thistogram: thistogram.c
	gcc -Wall -o thistogram thistogram.c -lrt -lpthread
	#./thistogram 0 10 10 3 input1.txt input2.txt input3.txt tresults.txt

clean:
	rm -fr phistogram thistogram *~ *.o
