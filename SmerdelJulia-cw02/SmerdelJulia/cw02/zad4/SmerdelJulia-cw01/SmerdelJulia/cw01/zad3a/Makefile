#-Wl, -rpath=sciezka/do/biblioteki

staticTest:
	make clean
	gcc ../zad2/main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryStatic  -o ../zad2/main
	cat test | ../zad2/./main > results_static.txt


sharedTest:
	# THE CODE BELOW SHOULD WORK - TESTED. IT'S NOT WORKING ON MY MAC (LIBRARY ISSUE), 
	# CONSEQUENTLY I COPIED LIBRARIES FROM ZAD1 TO ZAD3 FOLDER TO MAKE IT WORK

	# make clean
	# gcc ../zad2/main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryShared -o ../zad2/main
	# cat test | ../zad2/./main > results_shared.txt

	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc ../zad2/main.c -DNO_DYNAMIC_LIB -L. -lmyLibraryShared -o ../zad2/main
	cat test | ../zad2/./main > results_shared.txt


dynamicTest:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -o ../zad2/main ../zad2/main.c -ldl
	cat test | ../zad2/./main > results_dynamic.txt


clean:
	rm -f main ../zad2/main

cleanTxt:
	rm -f *.txt

cleanLibrary:
	rm -f *.so

summaryTests:
	grep -A 1000 "Full runtime of programm" results_static.txt | sed 's/ programm/& - static library/g' > report.txt

	grep -A 1000 "Full runtime of programm" results_shared.txt | sed 's/ programm/& - shared library/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_dynamic.txt | sed 's/ programm/& - dynamic library/g' >> report.txt

allTests:
	make cleanTxt
	make cleanLibrary
	make staticTest
	make sharedTest
	make dynamicTest
	make summaryTests
