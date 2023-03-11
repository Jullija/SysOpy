# Without creating libraries again - I'm using the ones from zad1
# -L - from which folder
# -l file

all:
	make static
	make shared
	make dynamic

static:
	#gcc main.c -DNO_DYNAMIC_LIB -L. -l:../zad1/libmyLibraryStatic.a -static -o main should work, but on my mac there is a problem :(

	gcc main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryStatic -o main
	

shared:
	#gcc main.c -DNO_DYNAMIC_LIB -L. -l:../zad1/libmyLibraryShared.so -o main should work, but on my mac there is a problem :(

	gcc main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryShared -o main

dynamic:
	gcc -o main main.c -ldl


clean:
	rm -f *.o *.a *.so main
