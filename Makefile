#COMPLILER USED
CC = g++

#VARIABLR
INCLUDE=./
LIB=./
 
#COMPILE FLAGS
ADD_INCL = -I$(INCLUDE)
ADD_LIB = -L$(LIB)

#LAST FLAG
CFLAGS = -Wall -g -lpthread -levent -std=c++11
CFLAGS += $(ADD_INCL)
LDFLAGS += $(ADD_LIB)

#SRC
SRC_PATH=./src

OBJS=$(patsubst %.cpp, %.o, $(wildcard $(SRC_PATH)/*.cpp))

#output
OUTPUT=proxy

BIN_DIR=./bin

#MAKE RULE
$(OUTPUT):$(OBJS)
	$(CC) -o $@ $^  $(CFLAGS) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c -o $@ $<  $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(OUTPUT) $(BIN_DIR)/$(OUTPUT)

install:
ifneq ($(strip $(OUTPUT)),)
	mv  $(OUTPUT) $(BIN_DIR)
endif
