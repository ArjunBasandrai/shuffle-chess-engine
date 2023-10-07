CC = gcc
ENGINE = shuffle
C = $(ENGINE).c
EXE = $(ENGINE).exe

all:
	$(CC) -Ofast $(C) -o $(EXE)
	./shuffle.exe

debug:
	$(CC) $(C) -o $(EXE)
	./shuffle.exe

compile:
	$(CC) -Ofast $(C) -o $(EXE)

distcheck:
	mkdir -p dist
	cp -r "src/" "dist/"
	rm -rf dist