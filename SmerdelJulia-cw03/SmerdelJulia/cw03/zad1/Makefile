
all:
	make clean
	make bigBang
	make test

bigBang:
	gcc -Wall main.c -o main

clean:
	rm -f *.o *.txt

test:
	echo 'MAKE 10 CHILDREN' > result.txt
	./main 10 >> result.txt

	echo 'MAKE 5 CHILDREN' >> result.txt
	./main 5 >> result.txt

