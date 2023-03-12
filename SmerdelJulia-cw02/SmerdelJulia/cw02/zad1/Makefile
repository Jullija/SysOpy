TOFIND = o
CHANGETO = a

all:
	make clean
	make lib
	make sys
	make diff

lib:
	gcc -Wall -D LIB_VERSION main.c -o main_lib
	./main_lib $(TOFIND) $(CHANGETO) test result_lib > pomiar_zad_1.txt

sys:
	gcc -Wall -D SYS_VERSION main.c -o main_sys
	./main_sys $(TOFIND) $(CHANGETO) test result_sys >> pomiar_zad_1.txt

diff:
	tr $(TOFIND) $(CHANGETO) <test> tmp
	diff -s tmp result_lib > difference.txt
	diff -s tmp result_sys >> difference.txt

clean:
	rm -f *.txt *.o result_lib result_sys main_lib main_sys tmp