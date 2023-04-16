all:
	make clean
	make compile

compile:
	gcc -Wall client.c -lrt -o client
	gcc -Wall server.c -lrt -o server

clean:
	rm -f *.o *.txt client server 