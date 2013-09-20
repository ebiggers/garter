OBJ := scan.o parse.o
CFLAGS := -Wall -O2
CC := clang

all: $(OBJ)

scan.c:scan.l
	flex -o $@ $+

scan.o:scan.c parse.h

parse.o:parse.c parse.h scan.h

parse.c:parse.y
	bison -d -o $@ $+

parse.h:parse.c
	@touch $@

clean:
	rm -f scan.c parse.c parse.h scan.h $(OBJ)

.PHONY: clean all
