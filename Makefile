CC = gcc
ENGINE = shuffle

C = $(ENGINE).c
EXE = $(ENGINE).exe

R_FLAGS = -Ofast

DISTDIR = dist
SRCDIR = src

all: __release_compile

debug: __debug_compile __run

__debug_compile:
	$(CC) $(C) -o $(EXE)

__release_compile:
	$(CC) $(R_FLAGS) $(C) -o $(EXE)

__run:
	./$(EXE)

dist:
	@echo "Building distribution tarball..."
	@mkdir -p "$(DISTDIR)"
	@cp -r "$(SRCDIR)/" "$(DISTDIR)/"
	@cp shuffle.c "$(DISTDIR)/"
	@cp Makefile "$(DISTDIR)/"
	@cp README.md "$(DISTDIR)/"
	@tar -czf $(DISTDIR).tar.gz "$(DISTDIR)"
	@rm -rf "$(DISTDIR)"

distcheck: dist
	@echo "Checking distribution tarball..."
	@tar -xzf $(DISTDIR).tar.gz
	@cd $(DISTDIR) && $(MAKE) all
	@cd $(DISTDIR) && $(MAKE) clean
	@rm -rf $(DISTDIR)
	@rm -f $(DISTDIR).tar.gz
	@echo "Test Successful!!"

clean:
	rm -rf $(EXE)

.PHONY: all clean dist distcheck
