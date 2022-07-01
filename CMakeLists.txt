
cmake_minimum_required(VERSION 3.10)

project(Zen CXX)

set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  set(is_debug_build ON)
else()
  set(is_debug_build OFF)
endif()

set(ZEN_NAMESPACE "zen" CACHE STRING "The namespace in which to embed Zen++")
set(ZEN_ENABLE_TESTS "${is_debug_build}" CACHE BOOL "Whether to generate the test infrastructure")
set(ZEN_ENABLE_ASSERTIONS "${is_debug_build}" CACHE BOOL "Force the compiler to generate assertions for certain invariants")

string(REPLACE "::" ";" zen_namespace_chunks "${ZEN_NAMESPACE}")

set(zen_namespace_start "")
set(zen_namespace_end "")
foreach(name IN LISTS zen_namespace_chunks)
  set(zen_namespace_start "${zen_namespace_start} namespace ${name} { ")
  set(zen_namespace_end "${zen_namespace_end} } ")
endforeach()

if (ZEN_ENABLE_ASSERTIONS)
  message("-- Compiling Zen++ with assertions enabled")
endif()

# This must be added before doing any other configuration so that
# changing something local doesn't trigger a GoogleTest rebuild
if (ZEN_ENABLE_TESTS)
  add_subdirectory(subprojects/googletest-release-1.11.0 EXCLUDE_FROM_ALL)
endif()

set(zen_sources
  src/program_options.cc
  src/json.cc
  src/filepath.cc
)

add_library(
  zen
  ${zen_sources}
)
target_compile_definitions(
  zen
  PUBLIC
  ZEN_NAMESPACE=${ZEN_NAMESPACE}
  "ZEN_NAMESPACE_START=${zen_namespace_start}"
  "ZEN_NAMESPACE_END=${zen_namespace_end}"
)
target_include_directories(
  zen
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCDIR}>
)
target_compile_features(zen INTERFACE cxx_std_17)

install(
  DIRECTORY "include"
  DESTINATION "${CMAKE_INSTALL_INCDIR}"
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
install(TARGETS zen EXPORT Zen)
install(EXPORT Zen NAMESPACE "${ZEN_NAMESPACE}::" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Zen")

if (ZEN_ENABLE_TESTS)
  add_executable(
    alltests
    test/zip_iterator.cc
    test/iterator_adaptor.cc
    test/program_options.cc
    test/meta.cc
    test/bytestring.cc
    test/filepath.cc
    test/json.cc
  )
  target_link_libraries(alltests zen gtest gtest_main)
  add_custom_target(check COMMAND alltests --gtest_color=yes COMMENT "Running tests")
endif()

