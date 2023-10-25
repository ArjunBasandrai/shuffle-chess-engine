CC = gcc
ENGINE = shuffle

C = $(ENGINE).c
EXE = $(ENGINE).exe

R_FLAGS = -Ofast

DISTDIR = dist
SRCDIR = src

SRCS = src/bit_manipulation.c \
		src/board.c \
		src/board_constants.c \
		src/magic_numbers.c \
		src/evaluation.c \
		src/fen.c \
		src/magic_num_generator.c \
		src/masks.c \
		src/movegen.c \
		src/moves_list.c \
		src/perft.c \
		src/pre_calculated_tables.c \
		src/random_unsigned.c \
		src/search.c \
		src/gettime.c \
		src/transposition_table.c \
		src/uci.c \
		src/zobrist.c \
		src/polyglot/polykeys.c \
		src/polyglot/polybook.c \
		src/books/book.c

all: __release_compile

debug: __debug_compile __run

__debug_compile:
	$(CC) -o $(EXE) $(C) $(SRCS)

__release_compile:
	$(CC) $(R_FLAGS) -o $(EXE) $(C) $(SRCS)

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
