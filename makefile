fl : fl.o process.o wordstore13.o
	gcc -Wall -o fl fl.o process.o wordstore13.o
	echo all done

fl.o : fl.c fl.h
	gcc -Wall -c fl.c

process.o : process.c fl.h ws13.h
	gcc -Wall -c process.c

wordstore13.o : wordstore13.c ws13.h
	gcc -Wall -c wordstore13.c

clean :
	rm -f *.o
	rm -f core
