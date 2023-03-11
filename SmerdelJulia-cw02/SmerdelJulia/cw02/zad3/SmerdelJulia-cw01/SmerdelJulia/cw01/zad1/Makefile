CC=gcc
CFLAGS = -Wall -pedantic #Wall - all errors, pedantic - every "clean code" mistake


all: static shared

static:
	$(CC) -c myLibrary.c                        #creating myLibrary.o
	ar crs libmyLibraryStatic.a myLibrary.o    #ar - archive, crs - c(reate new archive) r(add files to the archive) s(update the table of the archive)

shared:
	$(CC) $(CFLAGS) -shared -fPIC myLibrary.c -o libmyLibraryShared.so


clean:
	rm -f *.o *.a *.so