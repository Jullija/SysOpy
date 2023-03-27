all:
	make clean
	make onions
	make test


onions:
	gcc -Wall main.c -o main

clean:
	rm -f *.o *.txt

test:
	./main > report.txt
