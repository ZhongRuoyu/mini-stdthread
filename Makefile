CXXFLAGS = -O2
LDFLAGS =

COMMON_CXXFLAGS = -std=c++17 -pthread -Iinclude
COMMON_LDFLAGS = -pthread

EXAMPLE_SRCS = $(shell find examples -name *.cc | sort)
EXAMPLE_OBJS = $(EXAMPLE_SRCS:%.cc=out/%.o)
EXAMPLE_DEPS = $(EXAMPLE_SRCS:%.cc=out/%.d)
EXAMPLE_BINS = $(EXAMPLE_SRCS:%.cc=bin/%)
EXAMPLE_CXXFLAGS =
EXAMPLE_DEPFLAGS = -MT $@ -MMD -MP -MF out/examples/$*.d
EXAMPLE_LDFLAGS =

TEST_SRCS = $(shell find test -name *.cc | sort)
TEST_OBJS = $(TEST_SRCS:%.cc=out/%.o)
TEST_DEPS = $(TEST_SRCS:%.cc=out/%.d)
TEST_BINS = $(TEST_SRCS:%.cc=bin/%)
TEST_CXXFLAGS =
TEST_DEPFLAGS = -MT $@ -MMD -MP -MF out/test/$*.d
TEST_LDFLAGS =


.PHONY: all
all:

-include $(EXAMPLE_DEPS) $(TEST_DEPS)

.PHONY: examples
examples: $(EXAMPLE_BINS)

.PHONY: test
test: $(TEST_BINS)
	@set -e; \
		test -t 1 && red="\033[31m" || red=""; \
		test -t 1 && green="\033[32m" || green=""; \
		test -t 1 && yellow="\033[33m" || yellow=""; \
		test -t 1 && reset="\033[0m" || reset=""; \
		for test in $$(printf "%s\n" $^); do \
			echo "[ $${yellow}TESTING$${reset} ] $$test"; \
			( \
				$$test && \
				echo "[ $${green}PASSED$${reset}  ] $$test" \
			) || ( \
				echo "[ $${red}FAILED$${reset}  ] $$test" && \
				exit 1 \
			); \
		done
	@echo "All tests passed."

.PHONY: test-asan
test-asan:
	$(MAKE) CXXFLAGS="-O0 -g -fsanitize=address -fno-omit-frame-pointer" LDFLAGS="-fsanitize=address" test


out/examples/%.o: examples/%.cc
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(COMMON_CXXFLAGS) $(EXAMPLE_CXXFLAGS) $(EXAMPLE_DEPFLAGS) -c -o $@ $<

bin/examples/%: out/examples/%.o
	mkdir -p $(@D)
	$(CXX) -o $@ $< $(LDFLAGS) $(COMMON_LDFLAGS) $(EXAMPLE_LDFLAGS)


out/test/%.o: test/%.cc
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(COMMON_CXXFLAGS) $(TEST_CXXFLAGS) $(TEST_DEPFLAGS) -c -o $@ $<

bin/test/%: out/test/%.o
	mkdir -p $(@D)
	$(CXX) -o $@ $< $(LDFLAGS) $(COMMON_LDFLAGS) $(TEST_LDFLAGS)


.PHONY: clean
clean:
	rm -rf bin out
