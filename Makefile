CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -pthread

INCLUDES = -I./include

SRC_DIR = ./src
PARSER_DIR = $(SRC_DIR)/parser
OBJ_DIR = ./obj
BIN_DIR = ./bin

# List all source files manually or using wildcard (can extend later)
SRC_FILES = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/UdpReceiver.cpp \
	$(SRC_DIR)/MessageFactory.cpp \
	$(SRC_DIR)/Dispatcher.cpp \
	$(SRC_DIR)/KafkaProducer.cpp \
	$(SRC_DIR)/UdpMessageHandler.cpp

# Optional: add parser files (commented for now)
#PARSER_FILES = $(wildcard $(PARSER_DIR)/*.cpp)

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TARGET = $(BIN_DIR)/cboe_feed_handler

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

# Link object files to binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Compile src/*.cpp → obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Compile parser/*.cpp → obj/parser/*.o (if needed)
$(OBJ_DIR)/%.o: $(PARSER_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Create folders
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
