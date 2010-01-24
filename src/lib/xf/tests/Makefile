TOP_DIR = ../../../..
include $(TOP_DIR)/build/Makefile_common

TEST_SRCS = \
	TestXfRaise.cpp \
	TestXfLower.cpp \
	TestXfPartition.cpp \
	TestXfInstrument.cpp \
	TestXfLiveVars.cpp \
	TestXfLiveVarsWriter.cpp \
	TestXfFreeVars.cpp \
	$(NULL)

FOR_PARTITION = \
	TestPartBlock.slo \
	TestPartSeq.slo \
	TestPartIf.slo \
	TestPartNative.slo \
	$(NULL)

FOR_LIVE_VARS = \
	TestLive1.slo \
	TestLive2.slo \
	TestLive3.slo \
	TestLive4.slo \
	TestLiveLoop1.slo \
	TestLiveLoop2.slo \
	TestLiveLoop3.slo \
	TestLiveLoop4.slo \
	TestLiveLoop5.slo \
	TestLiveLoop6.slo \
	TestLiveLoop7.slo \
	TestLiveLoop8.slo \
	TestLiveLoop9.slo \
	TestLiveLoop10.slo \
	TestLiveIllum1.slo \
	TestLiveIllum2.slo \
	TestLiveIllum3.slo \
	TestLiveIllum4.slo \
	TestLiveIllum5.slo \
	TestLiveIllum6.slo \
	TestLiveRudyCSkin.slo \
	$(NULL)

ALSO_MAKE = $(FOR_PARTITION) $(FOR_LIVE_VARS)

CLEANUP = $(ALSO_MAKE)

SRC_DIR = src/lib/xf/tests
LIBS = libxf.a libir.a libslo.a libops.a libutil.a

include $(TOP_DIR)/build/Makefile_tests

$(OBJ_DIR)/TestXfPartition$(DOT_EXE): $(FOR_PARTITION)
$(OBJ_DIR)/TestXfLiveVars$(DOT_EXE): $(FOR_LIVE_VARS)
$(OBJ_DIR)/TestXfFreeVars$(DOT_EXE): $(FOR_LIVE_VARS)

%.slo: %.sl
	shader -back $<
