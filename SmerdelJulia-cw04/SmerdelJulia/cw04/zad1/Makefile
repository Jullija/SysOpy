all:
	make clean
	make swamp
	make test

swamp:
	gcc -Wall main.c -o main
	gcc -Wall child.c -o child

clean:
	rm -f *.o *.txt

test:
	echo 'Testing: ./main ignore\n\n ' > raport2.txt
	./main ignore  >> raport2.txt 

	echo '\nTesting: ./main handler\n\n ' >> raport2.txt
	./main handler  >> raport2.txt 

	echo '\nTesting: ./main mask\n\n' >> raport2.txt
	./main mask  >> raport2.txt 

	echo '\nTesting: ./main pending\n\n ' >> raport2.txt
	./main pending  >> raport2.txt 


	
