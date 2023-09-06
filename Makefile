all:
	gcc -oFast chess.c -o chess.exe
	./chess.exe

debug:
	gcc chess.c -o chess.exe
	./chess.exe