CXX_CFLAGS := -Werror -Wall -std=c++0x -O2 -pthread

SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

INC_FLAGS = -I$(INC_DIR) -I../aggregator/inc/ -I ../utils/inc/ -I ../side_libs/jsoncpp/include

SRC = $(wildcard $(SRC_DIR)/*.cpp)
INC = $(wildcard $(INC_DIR)/*.h)

NAME = csv_writer
TGT = $(BIN_DIR)/$(NAME)

all : $(TGT)

$(TGT) : $(SRC) $(INC)
	$(CXX) $(SRC) $(CXX_CFLAGS) $(INC_FLAGS) -Lbin/ -ljson_linux-gcc-4.6.3_libmt -o $(TGT)

.PHONY: clean

clean:
	rm -f $(TGT)

