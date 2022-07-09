CXXFLAGS += -std=c++17 -Iinclude
LDFLAGS += -pthread

EXAMPLE_SRCS = $(shell find examples -name *.cc | sort)
EXAMPLE_OBJS = $(EXAMPLE_SRCS:%.cc=out/%.o)
EXAMPLE_DEPS = $(EXAMPLE_SRCS:%.cc=out/%.d)
EXAMPLE_BINS = $(EXAMPLE_SRCS:%.cc=bin/%)
EXAMPLE_CXXFLAGS =
EXAMPLE_DEPFLAGS = -MT $@ -MMD -MP -MF out/examples/$*.d
EXAMPLE_LDFLAGS =


.PHONY: all
all:

-include $(EXAMPLE_DEPS)

.PHONY: examples
examples: $(EXAMPLE_BINS)


out/examples/%.o: examples/%.cc
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(EXAMPLE_CXXFLAGS) $(EXAMPLE_DEPFLAGS) -c -o $@ $<

bin/examples/%: out/examples/%.o
	mkdir -p $(@D)
	$(CXX) -o $@ $< $(LDFLAGS) $(EXAMPLE_LDFLAGS)


.PHONY: clean
clean:
	rm -rf bin out
