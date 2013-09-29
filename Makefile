CC     := clang
CFLAGS := -Wall -O0 -g
EXE    := garterc
FRONTEND_DIR := frontend
FRONTEND_OBJ := scan.o parse.o frontend.o
BISON := bison --debug
FLEX := flex

OBJ := $(addprefix $(FRONTEND_DIR)/,$(FRONTEND_OBJ)) main.o

all:$(EXE)

$(EXE):$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $+ $(LDLIBS)

$(FRONTEND_DIR)/scan.c:$(FRONTEND_DIR)/scan.l
	$(FLEX) -o $@ --header-file=$(FRONTEND_DIR)/scan.h $+

$(FRONTEND_DIR)/scan.o:$(FRONTEND_DIR)/scan.c $(FRONTEND_DIR)/parse.h

$(FRONTEND_DIR)/frontend.o:$(FRONTEND_DIR)/frontend.c $(FRONTEND_DIR)/parse.h $(FRONTEND_DIR)/scan.h

$(FRONTEND_DIR)/scan.h:$(FRONTEND_DIR)/scan.c
	@touch $@

$(FRONTEND_DIR)/parse.c:$(FRONTEND_DIR)/parse.y
	$(BISON) -d -o $@ $+

$(FRONTEND_DIR)/parse.o:$(FRONTEND_DIR)/parse.c $(FRONTEND_DIR)/scan.h

$(FRONTEND_DIR)/parse.h:$(FRONTEND_DIR)/parse.c
	@touch $@

clean:
	rm -f $(FRONTEND_DIR)/scan.c    \
	      $(FRONTEND_DIR)/parse.c   \
	      $(FRONTEND_DIR)/parse.h   \
	      $(FRONTEND_DIR)/scan.h    \
	      $(EXE) $(OBJ) tags cscope*

.PHONY: clean all
