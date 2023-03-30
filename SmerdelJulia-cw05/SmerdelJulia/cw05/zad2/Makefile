all:
	make clean
	make dwight
	make test

dwight:
	gcc -Wall main.c -o main

clean:
	rm -f *.o *.txt

test:
	./main 0.000001 1 > report.txt
	./main 0.000001 10 >> report.txt
	./main 0.001 1 >> report.txt
	./main 0.001 10 >> report.txt
	./main 0.01 1 >> report.txt
	./main 0.01 10 >> report.txt

	