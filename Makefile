all:
	gcc -Wredundant-decls -Ofast shuffle.c -o shuffle.exe
	./shuffle.exe

debug:
	gcc shuffle.c -o shuffle.exe
	./shuffle.exe

compile:
	gcc -Wredundant-decls -Ofast shuffle.c -o shuffle.exe