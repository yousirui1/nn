
RELEASE ?= 0 # default debug
UNICODE ?= 0 # default ansi


ifdef PLATFORM
	CROSS:=$(PLATFORM)-
else 
	CROSS:=
	PLATFORM:=linux
endif

ifeq ($(RELEASE),1)
	BUILD:=release
else
	BUILD:=debug
endif

KERNEL := $(shell uname -s)
ifeq ($(KERNEL),Linux)
	DEFINES += OS_LINUX
endif
ifeq ($(KERNEL),Darwin)
	DEFINES += OS_MAC
endif

#--------------------------------Compile-----------------------------
#
#--------------------------------------------------------------------

AR := $(CROSS)ar
CC := $(CROSS)gcc
CXX := $(CROSS)g++
CFLAGS += -Wall -fPIC
CXXFLAGS += -Wall -std=c++11
INCLUDES += $(BSP_INC_PATH)
DEPFLAGS = -MMD -MP -MF $(OUT_PATH)/$(*F).d

ifeq ($(RELEASE),1)
	CFLAGS += -Wall -O2 
	CXXFLAGS += $(CFLAGS)
	DEFINES += NDEBUG
else
	CFLAGS += -g -Wall
#   CFLAGS += -fsanitize=address
	CXXFLAGS += $(CFLAGS)
#   DEFINES += DEBUG _DEBUG
endif

# default don't export anything
CFLAGS += -fvisibility=hidden

COMPILE.CC = $(CC) $(addprefix -I,$(INCLUDES)) $(addprefix -D,$(DEFINES)) $(CFLAGS)
COMPILE.CXX = $(CXX) $(addprefix -I,$(INCLUDES)) $(addprefix -D,$(DEFINES)) $(CXXFLAGS)

#-------------------------Compile Output---------------------------
#
#--------------------------------------------------------------------
ifeq ($(UNICODE),1)
	OUT_PATH += unicode.$(BUILD).$(PLATFORM)
else
	OUT_PATH += $(BUILD).$(PLATFORM)
endif

# make output dir
$(shell mkdir -p $(OUT_PATH) > /dev/null)
$(shell mkdir -p $(ROOT)/build/lib > /dev/null)
$(shell mkdir -p $(ROOT)/build/bin > /dev/null)

#VPATH := $(ROOT)/src:

OBJECT_FILES := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE_FILES)))

#OBJECT_FILES := $(foreach file,$(SOURCE_FILES),$(OUT_PATH)/$(notdir $(file).o))


DEPENDENCE_FILES := $(OBJECT_FILES:%.o=%.d)
DEPENDENCE_FILES := $(foreach file,$(DEPENDENCE_FILES),$(OUT_PATH)/$(notdir $(file)))


#--------------------------Makefile Rules----------------------------
#
#--------------------------------------------------------------------
$(OUT_PATH)/$(OUT_FILE): $(OBJECT_FILES) $(STATIC_LIBS)
	@echo $(OUT_PATH)
ifeq ($(OUT_TYPE),0)
	$(CXX)  -o $@ -Wl,-rpath . $(LDFLAGS) $^ $(addprefix -L,$(LIB_PATHS)) $(addprefix -l,$(LIBS))
	@echo $(OUT_PATH)
#   $(CXX) $(OBJECT_FILES) -o $@ $(LDFLAGS)
else
ifeq ($(OUT_TYPE),1)
	$(CXX) -o $@ -shared -fPIC -rdynamic -Wl,-rpath . $(LDFLAGS) $^ $(addprefix -L,$(LIB_PATHS)) $(addprefix -l,$(LIBS))
else
	$(AR) -rc $@ $^
endif
endif
	@echo make ok, output: $(OUT_PATH)/$(OUT_FILE)

ifeq ($(OUT_TYPE),1)
	cp $(OUT_PATH)/$(OUT_FILE) $(ROOT)/build/lib
endif
ifeq ($(OUT_TYPE),2)
	cp $(OUT_PATH)/$(OUT_FILE) $(ROOT)/build/lib
endif
ifeq ($(OUT_TYPE),0)
	cp $(OUT_PATH)/$(OUT_FILE) $(ROOT)/build/bin
endif

%.o : %.c
#$(OUT_PATH)/%.o: %.c
	@echo $(OUT_PATH)
	$(COMPILE.CC) -c $(DEPFLAGS) -o $@ $<

%.o : %.cpp
#$(OUT_PATH)/%.o: %.cpp
	@echo $(OUT_PATH)
	$(COMPILE.CXX) -c $(DEPFLAGS) -o $@ $<

-include $(DEPENDENCE_FILES)

version.h : version.ver
	$(ROOT)/svnver.sh version.ver version.h
	
.PHONY: clean
clean:
	rm -rf $(OBJECT_FILES) $(OUT_PATH) $(DEPENDENCE_FILES) 
