include ../config.mk

CXXFLAGS+=-I ../shared
TARGET=../client.out

CXXFLAGS+=-std=c++11
LDFLAGS+=-lpthread

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
OBJS+=../shared.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	$(RM) $(TARGET)
	$(RM) *.o

.PHONY: all
.PHONY: clean
