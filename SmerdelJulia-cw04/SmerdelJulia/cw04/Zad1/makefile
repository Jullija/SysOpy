CC = gcc
CFLAGS = -Wall -pedantic
SRC = main.c

all:
	$(CC) $(CFLAGS) $(SRC) -o main
	$(CC) $(CFLAGS) child.c -o child

clean:
	rm -f main child
	make cleanTxt

cleanTxt:
	rm -f *.txt

test:
	make clean
	make

	echo 'Testing: ./main ignore ' >> raport2.txt.txt
	./main ignore  2>&1 | tee -a raport2.txt.txt > /dev/null

	echo '\n\nTesting: ./main handler  ' >> raport2.txt.txt
	./main handler  2>&1 | tee -a raport2.txt.txt > /dev/null

	echo '\n\nTesting: ./main mask' >> raport2.txt.txt
	./main mask  2>&1 | tee -a raport2.txt.txt > /dev/null

	echo '\n\nTesting: ./main pending ' >> raport2.txt.txt
	./main pending 2>&1 | tee -a raport2.txt.txt > /dev/null


	
