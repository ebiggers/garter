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

COMPILER_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(RUNTIME_CC_OBJ) garterc.o
INTERPRETER_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(RUNTIME_OBJ) garteri.o

ALL_CC_OBJ := $(FRONTEND_OBJ) $(BACKEND_OBJ) $(TEST_OBJ) $(RUNTIME_CC_OBJ) garterc.o garteri.o
ALL_CC_DEP := $(ALL_CC_OBJ:%.o=%.d)
ALL_OBJ := $(ALL_CC_OBJ) $(RUNTIME_GA_OBJ)
ALL_EXE := $(COMPILER_EXE) $(INTERPRETER_EXE) $(TEST_EXE)


all:$(COMPILER_EXE) $(INTERPRETER_EXE) $(RUNTIME_OBJ)

-include $(ALL_CC_DEP)

$(COMPILER_EXE):$(COMPILER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(INTERPRETER_EXE):$(INTERPRETER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(ALL_CC_OBJ): %.o : %.cc
	$(CXX) -o $@ -c $(CXXFLAGS) $(DEPFLAGS) $(CPPFLAGS) $<

$(RUNTIME_GA_OBJ): %.o: %.ga $(COMPILER_EXE)
	./garterc -c $<

test:$(TEST_EXE)
	for testprog in $(TEST_EXE); do		\
		$$testprog || exit $$?;		\
	done

$(TEST_EXE): %:%.o $(FRONTEND_OBJ) $(BACKEND_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(ALL_EXE) $(ALL_OBJ) $(ALL_CC_DEP) tags cscope*

.PHONY: clean all test
