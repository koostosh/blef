include ../config.mk

TARGET=../shared.a

CXXFLAGS+=-std=c++11
ARFLAGS=rvs

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	$(RM) $(TARGET)
	$(RM) *.o

.PHONY: all
.PHONY: clean
