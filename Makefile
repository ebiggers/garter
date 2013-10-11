CXX := clang++
CXXFLAGS := -Wall -Wextra -O0 -g
CPPFLAGS := -I. -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
LDLIBS := -lLLVMSupport -lpthread -ldl
COMPILER_EXE := garterc

FRONTEND_SRC := $(wildcard frontend/*.cc)
FRONTEND_OBJ := $(FRONTEND_SRC:%.cc=%.o)

TEST_SRC := $(wildcard test/*.cc)
TEST_OBJ := $(TEST_SRC:%.cc=%.o)
TEST_EXE := $(TEST_SRC:%.cc=%)

TOPLEVEL_SRC := $(wildcard *.cc)
TOPLEVEL_OBJ := $(TOPLEVEL_SRC:%.cc=%.o)

COMPILER_OBJ := $(FRONTEND_OBJ) $(TOPLEVEL_OBJ)

OBJ := $(COMPILER_OBJ) $(TEST_OBJ)

EXE := $(COMPILER_EXE) $(TEST_EXE)

all:$(COMPILER_EXE)

$(COMPILER_EXE):$(COMPILER_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

$(OBJ): %.o : %.cc
	$(CXX) -c -o $@ $(CXXFLAGS) $(CPPFLAGS) $+

test:$(TEST_EXE)
	for testprog in $(TEST_EXE); do		\
		$$testprog;			\
	done

$(TEST_EXE): %:%.o $(FRONTEND_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(EXE) $(OBJ) tags cscope*

.PHONY: clean all test
