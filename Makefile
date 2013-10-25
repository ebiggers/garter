SHELL := /bin/bash
CXX := clang++
LLVM_CXXFLAGS :=
LLVM_CPPFLAGS := $(shell llvm-config --cppflags)
LLVM_LDFLAGS  := $(shell llvm-config --ldflags)
#LLVM_LDLIBS   := $(shell llvm-config --libs)
LLVM_LDLIBS   := -lLLVM-3.3
CXXFLAGS := $(LLVM_CXXFLAGS) -Wall -Wextra -O2 -MMD -std=c++11
CPPFLAGS := $(LLVM_CPPFLAGS) -I.
LDFLAGS := $(LLVM_LDFLAGS)
LDLIBS := $(LLVM_LDLIBS)
COMPILER_EXE := garterc
INTERPRETER_EXE := garteri

FRONTEND_SRC := $(wildcard frontend/*.cc)
FRONTEND_OBJ := $(FRONTEND_SRC:%.cc=%.o)

BACKEND_SRC := $(wildcard backend/*.cc)
BACKEND_OBJ := $(BACKEND_SRC:%.cc=%.o)

RUNTIME_CC_SRC := $(wildcard runtime/*.cc)
RUNTIME_GA_SRC := $(wildcard runtime/*.ga)
RUNTIME_CC_OBJ := $(RUNTIME_CC_SRC:%.cc=%.o)
RUNTIME_GA_OBJ := $(RUNTIME_GA_SRC:%.ga=%.o)
RUNTIME_OBJ := $(RUNTIME_CC_OBJ) $(RUNTIME_GA_OBJ)

TEST_SRC := $(wildcard test/*.cc)
TEST_OBJ := $(TEST_SRC:%.cc=%.o)
TEST_EXE := $(TEST_SRC:%.cc=%)
TEST_SH  := $(wildcard test/*.sh)

COMPILER_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(COMPILER_EXE).o
INTERPRETER_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(RUNTIME_OBJ) $(INTERPRETER_EXE).o

ALL_CC_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(TEST_OBJ) \
			$(RUNTIME_CC_OBJ) $(COMPILER_EXE).o $(INTERPRETER_EXE).o
ALL_CC_DEP := $(ALL_CC_OBJ:%.o=%.d)
ALL_OBJ := $(ALL_CC_OBJ) $(RUNTIME_GA_OBJ)
ALL_EXE := $(COMPILER_EXE) $(INTERPRETER_EXE) $(TEST_EXE)


all:compiler interpreter

compiler:$(COMPILER_EXE) $(RUNTIME_OBJ)

interpreter:$(INTERPRETER_EXE)

-include $(ALL_CC_DEP)

$(COMPILER_EXE):$(COMPILER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(INTERPRETER_EXE):$(INTERPRETER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(ALL_CC_OBJ): %.o : %.cc
	$(CXX) -o $@ -c $(CXXFLAGS) $(DEPFLAGS) $(CPPFLAGS) $<

$(RUNTIME_GA_OBJ): %.o: %.ga $(COMPILER_EXE)
	./garterc -c $<

exe_tests:$(TEST_EXE)
	for testprog in $(TEST_EXE); do		\
		$$testprog || exit $$?;		\
	done

sh_tests:compiler interpreter
	for testprog in $(TEST_SH); do		\
		$$testprog || exit $$?;		\
	done

test:exe_tests sh_tests

check:test

$(TEST_EXE): %:%.o $(FRONTEND_OBJ) $(BACKEND_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(ALL_EXE) $(ALL_OBJ) $(ALL_CC_DEP) tags cscope* \
			test/garterc_and_garteri_Tests/*.{exe,out.o}

.PHONY: clean all test exec_tests sh_tests check compiler interpreter
