# Makefile for the ncurses game "NPONG"

# - Compiler - #
CC = gcc

# - Directories - #
IDIR = inc
ODIR = obj
BDIR = build

# - CFlags - #
COMPILER_FLAGS = -Wall -Wextra -Werror -O2 -std=c99 -I $(IDIR)

# - Libs - #
LINKER_FLAGS = -lncurses

# - Files - #
_DEPS = npong.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o npong.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: files npong

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $< $(COMPILER_FLAGS) -o $@

npong: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BDIR)/$@
	@echo "\n[SUCCESS]"

files:
	clear
	mkdir -p $(ODIR)
	mkdir -p build

.PHONY: clean

clean:
	rm -rf obj
	rm -rf build
	@echo "\n[SUCCESS]"
