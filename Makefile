CXX := clang++
CXXFLAGS := -Wall -Wextra -O0 -g -MMD -std=c++11
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

$(TEST_EXE): %:%.o $(FRONTEND_OBJ)
	$(CXX) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(EXE) $(OBJ) $(DEP) tags cscope*

.PHONY: clean all test
