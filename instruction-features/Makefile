CXX	         = g++

CXXFLAGS    += -g
CXXFLAGS    += -Wall
CXXFLAGS    += -std=c++11
CXXFLAGS    += -O2
CXXFLAGS    += -fmax-errors=3

DYNINST_ROOT = /usr/local
GPERF        = /usr
ELF_LIB      = /usr/lib/x86_64-linux-gnu
LIBDWARF     = /usr/local/lib

INC         += -I$(DYNINST_ROOT)/$(PLATFORM)/include
INC         += -I$(GPERF)/include

LIB         += -L$(DYNINST_ROOT)/$(PLATFORM)/lib
#LIB         += -L$(LIBDWARF) 
#LIB         += -L$(GPERF)/lib 
#LIB         += -L$(ELF_LIB)

DEP         += -lparseAPI
DEP         += -linstructionAPI 
#DEP         += -lsymtabAPI 
#DEP         += -ldynDwarf 
#DEP         += -ldwarf 
#DEP         += -ldynElf 
#DEP         += -lelf 
DEP         += -lcommon

PARSE_SRC    += main.cpp
PARSE_SRC    += binparse.cpp
PARSE_SRC    += util.cpp
PARSE_SRC    += CallTree.cpp

HEADERS      = $(wildcard *.h)

PARSE_OBJ    = $(PARSE_SRC:.cpp=.o)

OBJ         += $(PARSE_OBJ)

TARGETS     += feature-parse

.PHONY: all
all: $(TARGETS)

%.o: %.cpp $(HEADERS) Makefile
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

feature-parse: $(PARSE_OBJ)
	$(CXX) $(LIB) -o $@ $^ $(DEP) 

.PHONY: clean
clean:
	rm -f core core.* *.core $(OBJ) $(TARGETS)
