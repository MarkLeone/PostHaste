TOP_DIR = ../../..
include $(TOP_DIR)/build/Makefile_common

SRCS = \
	CgComponent.cpp \
	CgConst.cpp \
	CgDeserialize.cpp \
	CgInst.cpp \
	CgOptimize.cpp \
	CgShader.cpp \
	CgStmt.cpp \
	CgTypes.cpp \
	CgValue.cpp \
	CgVars.cpp \
	$(NULL)

# The generated bitcode for the plugin skeleton is saved in svn,
# since building it requires a RenderMan installation (for RslPlugin.h)
ALSO_MAKE = CgSkeleton.bc $(GEN_DIR)/lib/CgSkeleton.bc.o
ALSO_LINK = $(GEN_DIR)/lib/CgSkeleton.bc.o $(GEN_DIR)/lib/Ops.bc.o

SRC_DIR = src/lib/cg
LIB_NAME = libcg.a
CXXFLAGS += -I$(LLVM_DIR)/include

# We compile the plugin skeleton with limited optimization, since we don't
# want the various helper functions to be inlined.
LLVM_GXX_OPTS = -m64 -O1 -S -emit-llvm -I$(RMANTREE)/include -I..

include $(TOP_DIR)/build/Makefile_lib

%.bc: %.cpp $(TOP_DIR)/src/lib/ops/OpTypes.h
	llvm-g++ $(LLVM_GXX_OPTS) -o $(OBJ_DIR)/$<.s $<
	llvm-as -f -o $@ $(OBJ_DIR)/$<.s 

$(OBJ_DIR)/%.bc.cpp: %.bc
	@echo "// Auto-generated from" $< > $@
	@echo "#include <stddef.h>  // for size_t" >> $@
	@echo "unsigned char gCgSkeleton[] = {" >> $@
	hexdump -v -e '"" /1 "0x%02x" ",\n"' $< >> $@
	@echo "0x00 };" >> $@
	@echo "size_t gCgSkeletonSize = sizeof(gCgSkeleton)-1;" >> $@

$(GEN_DIR)/lib/%.bc.o: $(OBJ_DIR)/%.bc.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
