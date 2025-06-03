# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -O2 -pthread

# Directories
CUR_DIR  := $(shell pwd)
SRC_DIR  := $(CUR_DIR)
INCLUDE  := $(CUR_DIR)/../include
OBJDIR   := $(CUR_DIR)/obj
BINDIR   := $(CUR_DIR)/bin

# Libraries
LIBS     := -lyaml-cpp -lrdkafka

# Target
TARGET   := cboe_feed_handler

# Source files and object files
SOURCES := $(SRC_DIR)/main.cpp \
           $(SRC_DIR)/UdpReceiver.cpp \
           $(SRC_DIR)/KafkaProducer.cpp

OBJECTS := $(OBJDIR)/main.o \
           $(OBJDIR)/UdpReceiver.o \
           $(OBJDIR)/KafkaProducer.o

# Include flags
INCLUDES := -I$(INCLUDE)

# Rules

# Default target
all: $(BINDIR)/$(TARGET)

# Link
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile rules
$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Create bin and obj dirs if absent
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean
clean:
	rm -f $(OBJECTS) $(BINDIR)/$(TARGET)