staticTestO1:
	make clean
	gcc -O1 ../zad2/main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryStatic  -o ../zad2/main
	cat test | ../zad2/./main > results_static_O1.txt


sharedTestO1:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -O1 ../zad2/main.c -DNO_DYNAMIC_LIB -L. -lmyLibraryShared -o ../zad2/main
	cat test | ../zad2/./main > results_shared_O1.txt


dynamicTestO1:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -O1 -o ../zad2/main ../zad2/main.c -ldl
	cat test | ../zad2/./main > results_dynamic_O1.txt



staticTestO3:
	make clean
	gcc -O3 ../zad2/main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryStatic  -o ../zad2/main
	cat test | ../zad2/./main > results_static_O3.txt


sharedTestO3:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -O3 ../zad2/main.c -DNO_DYNAMIC_LIB -L. -lmyLibraryShared -o ../zad2/main
	cat test | ../zad2/./main > results_shared_O3.txt


dynamicTestO3:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -O3 -o ../zad2/main ../zad2/main.c -ldl
	cat test | ../zad2/./main > results_dynamic_O3.txt

staticTestOs:
	make clean
	gcc -Os ../zad2/main.c -DNO_DYNAMIC_LIB -L../zad1 -lmyLibraryStatic  -o ../zad2/main
	cat test | ../zad2/./main > results_static_Os.txt


sharedTestOs:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -Os ../zad2/main.c -DNO_DYNAMIC_LIB -L. -lmyLibraryShared -o ../zad2/main
	cat test | ../zad2/./main > results_shared_Os.txt


dynamicTestOs:
	make clean
	cp ../zad1/libmyLibraryShared.so .
	gcc -Os -o ../zad2/main ../zad2/main.c -ldl
	cat test | ../zad2/./main > results_dynamic_Os.txt


clean:
	rm -f main ../zad2/main

cleanTxt:
	rm -f *.txt

cleanLibrary:
	rm -f *.so


summaryTests:
	grep -A 1000 "Full runtime of programm" results_static_O1.txt | sed 's/ programm/& - static library O1/g' > report.txt

	grep -A 1000 "Full runtime of programm" results_shared_O1.txt | sed 's/ programm/& - shared library O1/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_dynamic_O1.txt | sed 's/ programm/& - dynamic library O1/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_static_Os.txt | sed 's/ programm/& - static library Os/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_shared_Os.txt | sed 's/ programm/& - shared library Os/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_dynamic_Os.txt | sed 's/ programm/& - dynamic library Os/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_static_O3.txt | sed 's/ programm/& - static library O3/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_shared_O3.txt | sed 's/ programm/& - shared library O3/g' >> report.txt

	grep -A 1000 "Full runtime of programm" results_dynamic_O3.txt | sed 's/ programm/& - dynamic library O3/g' >> report.txt

allTests:
	make cleanTxt
	make cleanLibrary

	make staticTestO1
	make sharedTestO1
	make dynamicTestO1

	make staticTestO3
	make sharedTestO3
	make dynamicTestO3

	make staticTestOs
	make sharedTestOs
	make dynamicTestOs

	make summaryTests



