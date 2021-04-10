
ZEN_HEADERS = $(wildcard include/*.hpp)
ZEN_SOURCES = $(wildcard src/*.cc)
ZEN_TEST_SOURCES = $(wildcard test/*.cc)

all: build/alltests
	./build/alltests

build/alltests: $(ZEN_HEADERS) $(ZEN_TEST_SOURCES)
	ninja -C build alltests

.PHONY: configure
configure:
	meson build

.PHONY: clean
clean:
	rm -rf build/
