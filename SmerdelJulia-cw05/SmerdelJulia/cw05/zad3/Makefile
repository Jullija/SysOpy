all:
	make clean
	make dwight
	make test

dwight:
	gcc -Wall main.c -o main
	gcc -Wall child.c -o child

clean:
	rm -f *.o *.txt child main

test:
	./main 0.000001 1 > report.txt
	./main 0.000001 10 >> report.txt
	./main 0.001 1 >> report.txt
	./main 0.001 10 >> report.txt
	./main 0.01 1 >> report.txt
	./main 0.01 10 >> report.txt
	./main 0.000000001 150 >> report.txt
	./main 0.000000001 300 >> report.txt

	