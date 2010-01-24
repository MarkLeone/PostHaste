TOP_DIR = ../../../..
include $(TOP_DIR)/build/Makefile_common

TEST_SRCS = \
	TestIRPos.cpp \
	TestIRType.cpp \
	TestIRTypesFactory.cpp \
	TestIRConst.cpp \
	TestIRVar.cpp \
	TestIRInst.cpp \
	TestIRStmt.cpp \
	TestIRShader.cpp \
	TestIRVarSet.cpp \
	TestIRVisitor.cpp \
	$(NULL)

OTHER_SRCS = \
	$(NULL)

SRC_DIR = src/lib/ir/tests
LIBS = libir.a libslo.a libops.a

include $(TOP_DIR)/build/Makefile_tests
