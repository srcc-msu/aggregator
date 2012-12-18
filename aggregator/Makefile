CC = g++
CFLAGS = -Werror -Wall -std=c++0x -g
LFLAGS = -shared -fPIC
#	g++ -shared -Wl,-soname,libmylib.so -o libmylib.so mylib.o

NAME=aggregator

SRC_DIR=src
INC_DIR=inc
BIN_DIR=bin

INC_FLAGS = -I$(INC_DIR)

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

TST = test/test.cpp

TGT=$(BIN_DIR)/lib$(NAME).so

LFLAGS := $(LFLAGS) -Wl,-soname,$(TGT)

all : $(TGT)

test : $(BIN_DIR)/test

$(BIN_DIR)/test : $(TGT) $(TST)
	$(CC) $(CFLAGS) $(TST) $(INC_FLAGS) -L$(BIN_DIR) -l$(NAME) -o $(BIN_DIR)/test

$(TGT) : $(SRC) $(INC)
	$(CC) $(CFLAGS) $(LFLAGS) $(INC_FLAGS) $(SRC) -o $(TGT) 

.PHONY: clean

clean:
	rm -f $(BIN_DIR)/*.so
	rm -f bin/test
	rm -f ruby/Makefile
