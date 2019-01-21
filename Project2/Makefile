all: syn_phistogram syn_thistogram;

syn_phistogram: syn_phistogram.c
	gcc -Wall -o syn_phistogram syn_phistogram.c -lrt -pthread
	#./phistogram 0 10 10 3 input1.txt input2.txt input3.txt presults.txt

syn_thistogram: syn_thistogram.c
	gcc -Wall -o syn_thistogram syn_thistogram.c -lrt -pthread
	#./thistogram 0 10 10 3 input1.txt input2.txt input3.txt tresults.txt

clean:
	rm -fr syn_phistogram syn_thistogram *~ *.o
