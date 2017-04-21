CXX	         = g++

CXXFLAGS    += -g
CXXFLAGS    += -Wall
CXXFLAGS    += -std=c++0x
CXXFLAGS    += -O2

DYNINST_ROOT = /usr/local
GPERF        = /usr
ELF_LIB      = /usr/lib/x86_64-linux-gnu

INC         += -I$(DYNINST_ROOT)/$(PLATFORM)/include
INC         += -I$(GPERF)/include

LIB         += -L$(DYNINST_ROOT)/$(PLATFORM)/lib
LIB         += -L$(LIBDWARF) 
LIB         += -L$(GPERF)/lib 
LIB         += -L$(ELF_LIB)

DEP         += -lparseAPI
DEP         += -linstructionAPI 
DEP         += -lsymtabAPI 
DEP         += -ldynDwarf 
DEP         += -ldwarf 
DEP         += -ldynElf 
DEP         += -lelf 
DEP         += -lcommon

EXTRACT_SRC += feature.cpp
EXTRACT_SRC += idiom.cpp
EXTRACT_SRC += operand.cpp
EXTRACT_SRC += lookup.cpp
EXTRACT_SRC += extractFeat.cpp
EXTRACT_SRC += graphlet.cpp
EXTRACT_SRC += supergraph.cpp
EXTRACT_SRC += colors.cpp
EXTRACT_SRC += FeatureAnalyzer.cpp
EXTRACT_SRC += IdiomAnalyzer.cpp
EXTRACT_SRC += GraphletAnalyzer.cpp

PRINT_SRC   += printIdioms.cpp
PRINT_SRC   += feature.cpp
PRINT_SRC   += idiom.cpp
PRINT_SRC   += lookup.cpp
PRINT_SRC   += operand.cpp

HEADERS      = $(wildcard *.h)

EXTRACT_OBJ  = $(EXTRACT_SRC:.cpp=.o)
PRINT_OBJ    = $(PRINT_SRC:.cpp=.o)

OBJ         += $(EXTRACT_OBJ)
OBJ         += $(PRINT_OBJ)

TARGETS     += extractFeat
TARGETS     += printFeat

.PHONY: all
all: $(TARGETS)

%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

extractFeat: $(EXTRACT_OBJ)
	$(CXX) $(LIB) -o $@ $^ $(DEP) 

printFeat: $(PRINT_OBJ)
	$(CXX) $(LIB) -o $@ $^ $(DEP)

.PHONY: clean
clean:
	rm -f core core.* *.core $(OBJ) $(TARGETS)
