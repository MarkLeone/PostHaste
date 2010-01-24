TOP_DIR = ../../../..
include $(TOP_DIR)/build/Makefile_common

TEST_SRCS = \
	TestSloFile.cpp \
	TestSloReadWrite.cpp \
	$(NULL)

OTHER_SRCS = \
	$(NULL)

CLEANUP = \
	temp.slo dented_copy.slo fake_copy.slo \
	$(NULL)

SRC_DIR = src/lib/slo/tests
LIBS = libslo.a libops.a libutil.a 

include $(TOP_DIR)/build/Makefile_tests
