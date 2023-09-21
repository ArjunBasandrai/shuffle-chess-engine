all:
	gcc -Wredundant-decls -Ofast chess.c -o chess.exe
	./chess.exe

debug:
	gcc chess.c -o chess.exe
	./chess.exe

compile:
	gcc -Wredundant-decls -Ofast chess.c -o chess.exe