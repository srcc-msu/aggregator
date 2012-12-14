CC = g++
CFLAGS = -Werror -Wall -std=c++0x -O2
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
	rm bin/test


#$(OBJ_DIR)agg.o : $(SRC_DIR)agg.cpp $(SRC_DIR)agg.h $(SRC_DIR)sensor_metainf.h
#	$(CC) $(CFLAGS) $(SRC_DIR)agg.cpp $(SRC_DIR)agg_api.cpp -o $(OBJ_DIR)agg.o

#$(OBJ_DIR)queue.o : $(SRC_DIR)queue.cpp $(SRC_DIR)queue.h
#	$(CC) $(CFLAGS) $(SRC_DIR)queue.cpp -o $(OBJ_DIR)queue.o

#$(OBJ_DIR)agg_api.o : $(SRC_DIR)agg_api.cpp $(SRC_DIR)agg_api.h
#	$(CC) $(CFLAGS) $(SRC_DIR)agg_api.cpp -o $(OBJ_DIR)agg_api.o

