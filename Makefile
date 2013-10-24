CXX := clang++
CC := clang
LLVM_CXXFLAGS :=
LLVM_CPPFLAGS := $(shell llvm-config --cppflags)
LLVM_LDFLAGS  := $(shell llvm-config --ldflags)
LLVM_LDLIBS   := $(shell llvm-config --libs)
CFLAGS := -Wall -Wextra -O0 -g
CXXFLAGS := $(LLVM_CXXFLAGS) $(CFLAGS) -MMD -std=c++11
CPPFLAGS := $(LLVM_CPPFLAGS) -I.
LDFLAGS := $(LLVM_LDFLAGS)
LDLIBS := $(LLVM_LDLIBS) -lpthread -ldl
COMPILER_EXE := garterc

FRONTEND_SRC := $(wildcard frontend/*.cc)
FRONTEND_OBJ := $(FRONTEND_SRC:%.cc=%.o)

BACKEND_SRC := $(wildcard backend/*.cc)
BACKEND_OBJ := $(BACKEND_SRC:%.cc=%.o)

RUNTIME_SRC := $(wildcard runtime/*.c)
RUNTIME_OBJ := $(RUNTIME_SRC:%.c=%.o)

TEST_SRC := $(wildcard test/*.cc)
TEST_OBJ := $(TEST_SRC:%.cc=%.o)
TEST_EXE := $(TEST_SRC:%.cc=%)

TOPLEVEL_SRC := $(wildcard *.cc)
TOPLEVEL_OBJ := $(TOPLEVEL_SRC:%.cc=%.o)

COMPILER_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(TOPLEVEL_OBJ)

OBJ := $(COMPILER_OBJ) $(TEST_OBJ)

DEP := $(OBJ:%.o=%.d)

EXE := $(COMPILER_EXE) $(TEST_EXE)


all:$(COMPILER_EXE)

-include $(DEP)

$(COMPILER_EXE):$(COMPILER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(OBJ): %.o : %.cc
	$(CXX) -o $@ -c $(CXXFLAGS) $(DEPFLAGS) $(CPPFLAGS) $<


test:$(TEST_EXE)
	for testprog in $(TEST_EXE); do		\
		$$testprog || exit $$?;		\
	done

$(TEST_EXE): %:%.o $(FRONTEND_OBJ) $(BACKEND_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(EXE) $(OBJ) $(DEP) tags cscope*

.PHONY: clean all test
