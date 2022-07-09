CXXFLAGS += -std=c++17 -Iinclude
LDFLAGS += -pthread

EXAMPLE_SRCS = $(shell find examples -name *.cc | sort)
EXAMPLE_OBJS = $(EXAMPLE_SRCS:%.cc=out/%.o)
EXAMPLE_BINS = $(EXAMPLE_SRCS:%.cc=bin/%)
EXAMPLE_CXXFLAGS += -fsanitize=address
EXAMPLE_LDFLAGS += -fsanitize=address


all: examples

examples: $(EXAMPLE_BINS)


out/examples/%.o: examples/%.cc
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(EXAMPLE_CXXFLAGS) -c -o $@ $^

bin/examples/%: out/examples/%.o
	mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(EXAMPLE_LDFLAGS) -o $@ $^


.PHONY: clean
clean:
	rm -rf bin out
