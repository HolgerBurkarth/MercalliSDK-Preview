# $CRT 27 Mai 2025 : hb
#
# $AUT Holger Burkarth
# $DAT >>makefile<< 27 Mai 2025  10:14:23 - (c) proDAD
#


# Compiler / Linker
CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wno-unknown-pragmas -O2
CXXFLAGS += -I../../../build/includes -I../../src -I.
CXXFLAGS += -D NDEBUG -D MERCALLISDK_LINK_DYNLIB
LDFLAGS := -L../../../bin/linux/Release
LDLIBS := -lprodadmercalli

# Files
SRCS := $(wildcard ../../src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
TARGET := ../../../bin/linux/Release/MercalliCli_customer

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
