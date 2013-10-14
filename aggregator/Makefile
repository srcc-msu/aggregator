CXX_CFLAGS = -Werror -Wall -std=c++0x -O2 -pthread 
CXX_LFLAGS = -shared -fPIC
#	g++ -shared -Wl,-soname,libmylib.so -o libmylib.so mylib.o

NAME=aggregator

SRC_DIR=src
INC_DIR=inc
BIN_DIR=bin

INC_FLAGS = -I$(INC_DIR) -I ../utils/inc

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

TST = test/test.cpp

TGT=$(BIN_DIR)/lib$(NAME).so

CXX_LFLAGS := $(CXX_LFLAGS) -Wl,-soname,lib$(NAME).so

all: $(TGT)

test: $(BIN_DIR)/test

$(BIN_DIR)/test: $(TGT) $(TST)
	$(CXX) $(TST) $(CXX_CFLAGS) $(INC_FLAGS) -L$(BIN_DIR) -l$(NAME) -o $(BIN_DIR)/test

$(TGT): $(SRC) $(INC)
	$(CXX) $(SRC) $(CXX_CFLAGS) $(CXX_LFLAGS) $(INC_FLAGS) -o $(TGT)

.PHONY: clean

clean:
	rm -f bin/*
