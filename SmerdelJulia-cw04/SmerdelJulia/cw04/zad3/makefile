CC = gcc
CFLAGS = -Wall -pedantic

all:
	$(CC) $(CFLAGS) sender.c -o sender
	$(CC) $(CFLAGS) catcher.c -o catcher
clean:
	rm -f main child
	make cleanTxt

cleanTxt:
	rm -f *.txt

test:
	make clean
	make
