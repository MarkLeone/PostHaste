TOP_DIR = ../../../..
include $(TOP_DIR)/build/Makefile_common

TEST_SRCS = \
	TestCgConst.cpp \
	TestCgValue.cpp \
	TestCgDeserialize.cpp \
	TestCgInst.cpp \
	TestCgShader.cpp \
	$(NULL)

FOR_CG_SHADER = \
	TestCgShader1.slo \
	TestCgShader2.slo \
	TestRudyCSkin.slo \
	$(NULL) 

# LLVM output differs slightly between platforms
TEST_PLATFORM = $(ARCH)
ALSO_MAKE = $(FOR_CG_SHADER)
CLEANUP = $(ALSO_MAKE)

CXXFLAGS += -I$(LLVM_DIR)/include
SRC_DIR = src/lib/cg/tests
LIBS = libcg.a libxf.a libir.a libslo.a libops.a libutil.a
SYS_LIBS += $(LLVM_LIBS)

include $(TOP_DIR)/build/Makefile_tests

$(OBJ_DIR)/TestCgShader$(DOT_EXE): $(FOR_CG_SHADER)

%.slo: %.sl
	shader -back $<
