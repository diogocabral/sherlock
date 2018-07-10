CC = gcc
CFLAGS = -Wall -ansi -pedantic -O2 -g
SOURCE = *.c *.h *akefile*
PROGRAM = sherlock

all:	$(PROGRAM)

$(PROGRAM):	sherlock.c
	$(CC) $(CFLAGS) -o sherlock sherlock.c

tarfile:
	tar cf $(PROGRAM).tar $(SOURCE)
	gzip $(PROGRAM).tar

zipfile:
	zip -k $(PROGRAM).zip $(SOURCE)

clean:
	rm -f *.o core $(PROGRAM)

