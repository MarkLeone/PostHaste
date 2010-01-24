TOP_DIR = ../../..
include $(TOP_DIR)/build/Makefile_common

SRCS = \
	XfFreeVars.cpp \
	XfInstrument.cpp \
	XfLiveVars.cpp \
	XfLower.cpp \
	XfPartition.cpp \
	XfPartitionInfo.cpp \
	XfRaise.cpp \
	$(NULL)

SRC_DIR = src/lib/xf
LIB_NAME = libxf.a

include $(TOP_DIR)/build/Makefile_lib

%.slo: %.sl
	shader -back $<
