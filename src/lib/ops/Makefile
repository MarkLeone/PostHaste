TOP_DIR = ../../..
include $(TOP_DIR)/build/Makefile_common

SRCS = \
	Opcode.cpp \
	OpcodeNames.cpp \
	OpInfo.cpp \
	OpVec3.cpp \
	OpVec4.cpp \
	OpMatrix3.cpp \
	OpMatrix4.cpp \
	$(NULL)

SRC_DIR = src/lib/ops
LIB_NAME = libops.a
ALSO_MAKE = $(GEN_DIR)/lib/Ops.bc.o

CLANG_OPTS = -m64 -O2 -c -emit-llvm -I$(RMANTREE)/include -I..

include $(TOP_DIR)/build/Makefile_lib

$(OBJ_DIR)/%.bc: %.cpp OpTypes.h
	clang $(CLANG_OPTS) -o $@ $<

%.bc.cpp: %.bc
	@echo "// Auto-generated from" $< > $@
	@echo "#include <stddef.h>  // for size_t" >> $@
	@echo "unsigned char gShadeops[] = {" >> $@
	hexdump -v -e '"" /1 "0x%02x" ",\n"' $< >> $@
	@echo "0x00 };" >> $@
	@echo "size_t gShadeopsSize = sizeof(gShadeops)-1;" >> $@

$(GEN_DIR)/lib/Ops.bc.o: $(OBJ_DIR)/Ops.bc.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
