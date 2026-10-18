# Tests hôtes (parseur de catalogue, sans réseau ni 3DS).
#
#   make -C tests/host
CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -O1 -g
CPPFLAGS := -I../../source

BIN     := test_catalog
SRC     := ../../source/json.c ../../source/catalog.c test_catalog.c

all: run

$(BIN): $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(SRC)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
