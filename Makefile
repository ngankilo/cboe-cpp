CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -pthread

# Detect OS (Darwin = macOS)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    YAML_INCLUDE = -I/opt/homebrew/include
    YAML_LIB     = -L/opt/homebrew/lib
else
    YAML_INCLUDE = -I/usr/local/include
    YAML_LIB     = -L/usr/local/lib
endif
INCLUDES = -I./include $(YAML_INCLUDE)
LIBS     = $(YAML_LIB) -lyaml-cpp -lrdkafka

TARGET = cboe_feed_handler

SRCDIR = ./src
PARSERDIR = $(SRCDIR)/parser
OBJDIR = ./obj
BINDIR = ./bin
# Libraries

SRC_SOURCES = main.cpp UdpReceiver.cpp KafkaProducer.cpp SymbolIdentifier.cpp MessageFactory.cpp
#PARSER_SOURCES = AddOrder.cpp AuctionSummary.cpp AuctionUpdate.cpp CalculatedValue.cpp DeleteOrder.cpp \
#                 EndOfSession.cpp GapLogin.cpp GapRequest.cpp GapResponse.cpp LoginResponse.cpp \
#                 ModifyOrder.cpp OrderExecuted.cpp OrderExecutedAtPrice.cpp ReduceSize.cpp \
#                 SequenceUnitHeader.cpp Trade.cpp TradeBreak.cpp TradingStatus.cpp UnitClear.cpp

SOURCES = $(SRC_SOURCES) $(PARSER_SOURCES)
SRC_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC_SOURCES))
PARSER_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(PARSER_SOURCES))
OBJS = $(SRC_OBJS) $(PARSER_OBJS)

all: $(BINDIR)/$(TARGET)
#$(BINDIR)/udp_example
#
#$(BINDIR)/udp_example: $(filter-out $(OBJDIR)/main.o,$(OBJS)) $(OBJDIR)/main_udp_example.o
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/%.o: $(PARSERDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp ./include/UdpReceiver.hpp ./include/pitch/seq_unit_header.h ./include/pitch/message_factory.h ./include/pitch/message.h ./include/DisruptorRouter.hpp
$(OBJDIR)/UdpReceiver.o: $(SRCDIR)/UdpReceiver.cpp ./include/UdpReceiver.hpp
$(OBJDIR)/CboeParser.o: ./include/pitch/message_factory.h ./include/pitch/message_dispatcher.h ./include/pitch/seq_unit_header.h
#$(OBJDIR)/SequenceUnitHeader.o: $(PARSERDIR)/SequenceUnitHeader.cpp ./include/pitch/seq_unit_header.h ./include/pitch/message_dispatcher.h

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(TARGET)

.PHONY: all clean