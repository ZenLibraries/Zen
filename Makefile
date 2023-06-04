
#export CLANG_INCLUDE_DIR ?= /usr/lib/clang/13.0.0/include
#export CLANG_LIBRARY_PATH ?= $(HOME)/Projects/llvm-project/install/usr/local/lib

ZEN_HEADERS = $(wildcard include/zen/*.hpp)
ZEN_SOURCES = $(wildcard src/*.cc)
ZEN_TEST_SOURCES = $(wildcard test/*.cc)

all: build/alltests
	./build/alltests

build/alltests: build/build.ninja $(ZEN_HEADERS) $(ZEN_SOURCES) $(ZEN_TEST_SOURCES)
	ninja -C build alltests

build/build.ninja: CMakeLists.txt
	cmake \
		-B build \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_CXX_COMPILER=clang++ \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-G Ninja

# build/build.ninja: meson.build
# 	meson build -Dzen_enable_tests=true -Dzen_enable_assertions=true

.PHONY: debug
debug: build/alltests
	lldb build/alltests -- --gtest_break_on_failure

.PHONY: clean
clean:
	ninja -C build clean

.PHONY: clean
distclean:
	rm -rf build/
