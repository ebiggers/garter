OBJ := scan.o parse.o frontend.o main.o
CFLAGS := -Wall -O2
CC := clang
EXE := garterc

all:$(EXE)

$(EXE):$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $+ $(LDLIBS)

scan.c:scan.l
	flex -o $@ --header-file=scan.h $+

scan.h:scan.c
	@touch $@

scan.o:scan.c parse.h

frontend.o:frontend.c

parse.o:parse.c parse.h scan.h

parse.c:parse.y
	bison -d -o $@ $+

parse.h:parse.c
	@touch $@

clean:
	rm -f scan.c parse.c parse.h scan.h $(EXE) $(OBJ) tags cscope*

.PHONY: clean all
