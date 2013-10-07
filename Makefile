CXX    := clang++
CXXFLAGS := -Wall -O0 -g -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
EXE    := garterc
FRONTEND_DIR := frontend
FRONTEND_OBJ := GarterLexer.o

OBJ := $(addprefix $(FRONTEND_DIR)/,$(FRONTEND_OBJ))

all:$(EXE)

clean:
	rm -f $(EXE) $(OBJ) tags cscope*

.PHONY: clean all
