CC = gcc
ENGINE = shuffle

C = $(ENGINE).c
EXE = $(ENGINE).exe

DISTDIR = dist
SRCDIR = src

all:
	$(CC) -Ofast $(C) -o $(EXE)
	./shuffle.exe

debug:
	$(CC) $(C) -o $(EXE)
	./shuffle.exe

compile:
	$(CC) -Ofast $(C) -o $(EXE)

distcheck:
	mkdir -p "$(DISTDIR)"
	cp -r "$(SRCDIR)/" "$(DISTDIR)/"
	cp shuffle.c "$(DISTDIR)/"
	cp Makefile "$(DISTDIR)/"
	cp README.md "$(DISTDIR)/"
	tar -czf $(DISTDIR).tar.gz "$(DISTDIR)"
	rm -rf "$(DISTDIR)"

	tar -xzf $(DISTDIR).tar.gz
	cd $(DISTDIR) && $(MAKE) all
	cd $(DISTDIR) && $(MAKE) clean
	rm -rf $(DISTDIR)
	rm -f $(DISTDIR).tar.gz