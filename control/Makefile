CXX_CFLAGS := -Werror -Wall -std=c++0x -O2 -pthread

SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

INC_FLAGS = -I$(INC_DIR) -I../aggregator/inc/ -I ../utils/inc/

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

NAME = control

TGT = $(BIN_DIR)/$(NAME)

all: $(TGT)

$(TGT): $(SRC) $(INC)
	$(CXX) $(SRC) $(CXX_CFLAGS) $(INC_FLAGS) -o $(TGT)

init:
	bin/control init_stream /tmp/proxy 93.180.9.182 4544

.PHONY: clean

clean:
	rm -f bin/*
