# Makefile for the ncurses game "NPONG"

# - Compiler - #
CC = gcc

# - Directories - #
IDIR = inc
ODIR = obj
BDIR = build

# - CFlags - #
COMPILER_FLAGS = -Wall -Wextra -O2 -I $(IDIR)

# - Libs - #
LINKER_FLAGS = -lncurses

# - Files - #
_DEPS = npong.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o npong.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: files npong

$(ODIR)/%.o: %.c
	$(CC) -c $< $(COMPILER_FLAGS) -o $@

npong: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BDIR)/$@
	@echo "\n[SUCCESS]"

files:
	mkdir -p $(ODIR)
	mkdir -p build

.PHONY: clean

clean:
	rm -f obj/*.o
	rm -f build/*
	@echo "\n[SUCCESS]"
