# Tests hôtes (parseur de catalogue, configuration, sans réseau ni 3DS).
#
#   make -C tests/host
#
# gnu11 : même famille que le standard implicite de devkitARM, dont la libc
# fournit strtok_r() utilisée par la couche configuration.
CC      ?= cc
CFLAGS  ?= -std=gnu11 -Wall -Wextra -Werror -O1 -g
CPPFLAGS := -I../../source

BINS    := test_catalog test_fsutil

all: run

test_catalog: ../../source/json.c ../../source/catalog.c test_catalog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

test_fsutil: ../../source/fsutil.c test_fsutil.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

run: $(BINS)
	@set -e; for t in $(BINS); do echo "== $$t =="; ./$$t; done

clean:
	rm -f $(BINS)

.PHONY: all run clean
