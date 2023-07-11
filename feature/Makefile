
#--------------------------------Output------------------------------
#
# OUT_TYPE: 0-exe, 1-dll, 2-static
#--------------------------------------------------------------------
OUT_TYPE = 0
OUT_FILE = test
ROOT=.
3RD_DIR = $(ROOT)/3rdparty

#-------------------------------Include------------------------------
#
# INCLUDES = $(addprefix -I,$(INCLUDES)) # add -I prefix
#--------------------------------------------------------------------
INCLUDES = $(ROOT)/include 


#-------------------------------Source-------------------------------
#
#--------------------------------------------------------------------
SOURCE_PATHS = src 
SOURCE_FILES = $(foreach dir,$(SOURCE_PATHS),$(wildcard $(dir)/*.cpp))
SOURCE_FILES += $(foreach dir,$(SOURCE_PATHS),$(wildcard $(dir)/*.c))

#-----------------------------Library--------------------------------
#
# LIB_PATHS = $(addprefix -L,$(LIBPATHS)) # add -L prefix
#--------------------------------------------------------------------
LIB_PATHS = $(ROOT)/lib 

ifdef RELEASE
# relase library path
LIB_PATHS += 
else
LIB_PATHS +=
endif

LIBS =   


STATIC_LIBS =

#-----------------------------DEFINES--------------------------------
#
# DEFINES := $(addprefix -D,$(DEFINES)) # add -L prefix
#--------------------------------------------------------------------
# ONNX_MODEL BMRL_MODEL RKNN_MODEL FLOATING_POINT -> ONNXRUNTIME
DEFINES =  FLOATING_POINT  


#ONNX_MODEL
include $(ROOT)/gcc.mk
