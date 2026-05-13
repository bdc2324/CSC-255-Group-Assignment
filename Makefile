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

clean:
	rm -f $(TARGET)

.PHONY: all clean
