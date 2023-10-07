all:
	gcc  -Ofast shuffle.c -o shuffle.exe
	./shuffle.exe

debug:
	gcc shuffle.c -o shuffle.exe
	./shuffle.exe

compile:
	gcc -Ofast shuffle.c -o shuffle.exe