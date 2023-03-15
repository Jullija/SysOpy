all:
	make clean
	make rickroll
	make test

rickroll:
	gcc -Wall main.c -o main

clean:
	rm -f *.o result.txt

test:
	echo 'Empty directory' > result.txt
	./main ./1NeverGonna >> result.txt

	echo '\n\nCouple files directory' >> result.txt
	./main ./GiveYou >> result.txt

	echo '\n\nA lot of files directory' >> result.txt
	./main ./Up >> result.txt
