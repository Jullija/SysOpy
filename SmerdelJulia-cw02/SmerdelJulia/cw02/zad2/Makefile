all:
	make clean
	make one
	make more
	make diff


one:
	gcc -Wall -D ONE_AT_TIME main.c -o main_one
	./main_one test.txt result_one > pomiar_zad_2.txt


more:
	gcc -Wall -D MORE_AT_TIME main.c -o main_more
	./main_more test.txt result_more >> pomiar_zad_2.txt

diff:
	diff -s result_more result_one > difference.txt

clean:
	rm -f *.o main_more main_one result_more result_one result pomiar_zad_2.txt difference.txt