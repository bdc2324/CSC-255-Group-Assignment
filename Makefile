CXX      = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic
TARGET   = battleship
SRCDIR   = src
INCDIR   = include
SRCS     = $(SRCDIR)/Board.cpp \
           $(SRCDIR)/HumanPlayer.cpp \
           $(SRCDIR)/AIPlayer.cpp \
           $(SRCDIR)/Game.cpp \
           $(SRCDIR)/main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o $(TARGET) $(SRCS)

test:
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o tests/run_tests tests/tests.cpp $(SRCDIR)/Board.cpp
	./tests/run_tests

clean:
	rm -f $(TARGET) tests/run_tests

.PHONY: all test clean
