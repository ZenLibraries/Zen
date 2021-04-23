
ZEN_HEADERS = $(wildcard include/zen/*.hpp)
ZEN_SOURCES = $(wildcard src/*.cc)
ZEN_TEST_SOURCES = $(wildcard test/*.cc)

all: build/alltests
	./build/alltests

build/alltests: build/build.ninja $(ZEN_HEADERS) $(ZEN_SOURCES) $(ZEN_TEST_SOURCES)
	ninja -C build alltests

build/build.ninja: meson.build
	meson build

.PHONY: debug
debug: build/alltests
	lldb build/alltests -- --gtest_break_on_failure

.PHONY: clean
clean:
	rm -rf build/
