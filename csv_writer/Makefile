CXX_CFLAGS := -Werror -Wall -std=c++0x -O2 -pthread

SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

INC_FLAGS = -I$(INC_DIR) -I../aggregator/inc

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

NAME = csv_writer
TGT = $(BIN_DIR)/$(NAME)

all : $(TGT)

$(TGT) : $(SRC) $(INC)
	$(CXX) $(SRC) $(CXX_CFLAGS) $(INC_FLAGS) -o $(TGT)

.PHONY: clean

clean:
	rm -f $(TGT)

