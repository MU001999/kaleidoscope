DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp, ${DIR_OBJ}/%.o, $(notdir ${SRC}))

TARGET = toy

BIN_TARGET = $(DIR_BIN)/$(TARGET)

CC = g++
CPPFLAGS = -g -std=c++17

${BIN_TARGET}: ${OBJ} | ${DIR_BIN}
	${CC} ${OBJ} `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` -O3 -rdynamic -o $@

${DIR_OBJ}/%.o: ${DIR_SRC}/%.cpp | ${DIR_OBJ}
	${CC} ${CPPFLAGS} -c $< -o $@

${DIR_BIN}:
	mkdir $@

${DIR_OBJ}:
	mkdir $@

.PHONY: clean
clean:
	rm -rf ${DIR_OBJ}/*.o
