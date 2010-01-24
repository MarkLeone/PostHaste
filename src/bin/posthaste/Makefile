TOP_DIR = ../../..
include $(TOP_DIR)/build/Makefile_common

SRCS = Main.cpp getopt.cpp
SRC_DIR = src/bin/posthaste
EXE_NAME = posthaste
LIBS = libcg.a libxf.a libir.a libslo.a libops.a libutil.a 
CXXFLAGS += -I$(LLVM_DIR)/include
SYS_LIBS += $(LLVM_LIBS)

include $(TOP_DIR)/build/Makefile_bin
