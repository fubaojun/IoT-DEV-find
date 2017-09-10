OS = $(shell uname)
PLATFORM ?= Ubuntu
ifeq "$(PLATFORM)" "LOONGSON"
CC = mipsel-linux-gcc
else
CC = gcc
endif
#CFLAGS += -O2 -static
CFLAGS += -O0

PROGRAMDIR := .
PROGRAM := $(PROGRAMDIR)/iot-find-server
OBJDIR := ./obj
OBJ := $(PROGRAMDIR)/iot-find-server.o $(PROGRAMDIR)/wrap.o 


all:$(PROGRAM)

$(PROGRAM):$(OBJ)
	$(CC) $(CFLAGS) $^ -o $(PROGRAM)
	@echo "---1---"
	@echo "====Build Successfully!===="
	tree 

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@
	@echo "---2---"

install:
	ncat -l4p 83 < $(PROGRAM)

clean:
	$(RM) *.o $(PROGRAM) $(OBJ)
