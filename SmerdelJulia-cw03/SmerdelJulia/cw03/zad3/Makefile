all:
	make clean
	make butcher
	make test

butcher:
	gcc -Wall main.c -o main

clean:
	rm -f *.o *.txt

test:
	echo 'Test: ./main tests Never' > result.txt
	./main tests Never >> result.txt

	echo 'Test: ./main tests cholibka' >> result.txt
	./main tests cholibka >> result.txt

	echo 'Test: ./main tests \#include' >> result.txt
	./main tests \#include >> result.txt

	echo 'Test: ./main . tararariraratonga' >> result.txt
	./main . tararariraratonga  >> result.txt

	echo 'Test: ./main ../.. ekskjuzmi' >> result.txt
	./main ../.. ekskjuzmi  >> result.txt