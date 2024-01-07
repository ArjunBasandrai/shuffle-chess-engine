CC = clang
ENGINE = shuffle

C = $(ENGINE).c

ifndef v
v=test
endif

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
		src/see.c \
		src/see_test.c \
		src/gettime.c \
		src/transposition_table.c \
		src/uci.c \
		src/zobrist.c \
		src/polyglot/polykeys.c \
		src/polyglot/polybook.c \
		src/threading/tinycthread.c 
		
all: __windows_compile

windows: __windows_compile

linux: __liux_compile

apple_arm: __apple_arm_compile

__windows_compile:
	$(CC) -Ofast -o shuffle_$(v).exe $(C) $(SRCS)

__linux_compile:
	$(CC) -Ofast -arch x86_64 -o shuffle_$(v) $(C) $(SRCS)

__apple_arm_compile:
	$(CC) -Ofast -arch arm64 -o shuffle_$(v) $(C) $(SRCS)

dist_windows:
	@echo "Building distribution tarball..."
	@mkdir -p "$(DISTDIR)"
	@cp -r "$(SRCDIR)/" "$(DISTDIR)/"
	@cp shuffle.c "$(DISTDIR)/"
	@cp Makefile "$(DISTDIR)/"
	@cp README.md "$(DISTDIR)/"
	@tar -czf $(DISTDIR).tar.gz "$(DISTDIR)"
	@rm -rf "$(DISTDIR)"

dist_apple:
	@echo "Building distribution tarball..."
	@mkdir -p "$(DISTDIR)"
	@mkdir -p "$(DISTDIR)/$(SRCDIR)"
	@cp -r "$(SRCDIR)/" "$(DISTDIR)/$(SRCDIR)/"
	@cp shuffle.c "$(DISTDIR)/"
	@cp Makefile "$(DISTDIR)/"
	@cp README.md "$(DISTDIR)/"
	@tar -czf $(DISTDIR).tar.gz "$(DISTDIR)"
	@rm -rf "$(DISTDIR)"

dist_linux:
	@echo "Building distribution tarball..."
	@mkdir -p "$(DISTDIR)"
	@cp -r "$(SRCDIR)/" "$(DISTDIR)/"
	@cp shuffle.c "$(DISTDIR)/"
	@cp Makefile "$(DISTDIR)/"
	@cp README.md "$(DISTDIR)/"
	@tar -czf $(DISTDIR).tar.gz "$(DISTDIR)"
	@rm -rf "$(DISTDIR)"

distcheck_windows: dist_windows
	@echo "Checking distribution tarball..."
	@tar -xzf $(DISTDIR).tar.gz
	@cd $(DISTDIR) && $(MAKE) windows
	@cd $(DISTDIR) && $(MAKE) clean
	@rm -rf $(DISTDIR)
	@rm -f $(DISTDIR).tar.gz
	@echo "Test Successful!!"

distcheck_apple: dist_apple
	@echo "Checking distribution tarball..."
	@tar -xzf $(DISTDIR).tar.gz
	@cd $(DISTDIR) && $(MAKE) apple_arm
	@cd $(DISTDIR) && $(MAKE) clean
	@rm -rf $(DISTDIR)
	@rm -f $(DISTDIR).tar.gz
	@echo "Test Successful!!"

distcheck_linux: dist_linux
	@echo "Checking distribution tarball..."
	@tar -xzf $(DISTDIR).tar.gz
	@cd $(DISTDIR) && $(MAKE) linux
	@cd $(DISTDIR) && $(MAKE) clean
	@rm -rf $(DISTDIR)
	@rm -f $(DISTDIR).tar.gz
	@echo "Test Successful!!"

clean:
	rm -rf $(EXE)

.PHONY: all clean dist distcheck
