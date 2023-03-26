CC = gcc
CFLAGS = -Wall -pedantic
SRC = main.c

all:
	$(CC) $(CFLAGS) $(SRC) -o main
	# $(CC) $(CFLAGS) child.c -o child

clean:
	rm -f main child
	make cleanTxt

cleanTxt:
	rm -f *.txt

test:
	make clean
	make
