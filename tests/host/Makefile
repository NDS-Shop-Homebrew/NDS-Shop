# Host-side tests (catalog parser, config, no network or 3DS).
#
#   make -C tests/host

CC      ?= cc
CFLAGS  ?= -std=gnu11 -Wall -Wextra -Werror -O1 -g
CPPFLAGS := -I../../include

BINS    := test_catalog test_fsutil test_i18n

all: run

test_catalog: ../../source/catalog/json.c ../../source/catalog/catalog.c test_catalog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

test_fsutil: ../../source/core/fsutil.c test_fsutil.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

test_i18n: ../../source/core/i18n.c test_i18n.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

run: $(BINS)
	@set -e; for t in $(BINS); do echo "== $$t =="; ./$$t; done

clean:
	rm -f $(BINS)

.PHONY: all run clean
