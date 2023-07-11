MODULE=fft
TOPDIR=../../..
include $(TOPDIR)/Rules.mk
SRCDIR=$(TOPDIR)/src
OBJ_OUTPUT_DIR=$(TOPDIR)/objs
LIB_OUTPUT_DIR=$(TOPDIR)/libs

SRC = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o, $(SRC))

INCLUDE_FLAGS    = -I$(SRCDIR) -I./
LD_FLAGS         = -lm 

LD_LIBS          = -L. -l$(MODULE)
TARGET		 	 = lib$(MODULE).so

all: $(OBJS)
	#$(AR) -crv $(TARGET)  $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS)
	mv $(TARGET) $(LIB_OUTPUT_DIR)
	mv $(OBJS) $(OBJ_OUTPUT_DIR)

%.o:%.c
	#$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@
	$(CC) -fPIC $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

.phony: clean

clean:
	@rm -f $(LIB_OUTPUT_DIR)/$(OBJS) $(LIB_OUTPUT_DIR)/$(TARGET)
