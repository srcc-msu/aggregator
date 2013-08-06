CXX_CFLAGS := -Werror -Wall -std=c++0x -O2 -pthread

SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

INC_FLAGS = -I$(INC_DIR) -I../aggregator/inc/ -I ../utils/inc/

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

NAME = hr_dumper
TGT = $(BIN_DIR)/$(NAME)

all: $(TGT)

$(TGT): $(SRC) $(INC)
	$(CXX) $(SRC) $(CXX_CFLAGS) $(INC_FLAGS) -Lbin/ -o $(TGT)

.PHONY: clean

clean:
	rm -f bin/*
