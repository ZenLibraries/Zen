
project('zen2', 'cpp', version: '0.0.1')

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')

zen_lib = static_library(
  'zen',
  cpp_args: [ '-std=c++17' ],
)

zen_dep = declare_dependency(
  include_directories: 'include',
  compile_args: [ '-std=c++17' ],
  link_with: [ zen_lib ],
)

executable(
  'alltests',
  'test/zip_iterator.cc',
  'test/program_options.cc',
  include_directories: 'include',
  dependencies: [ gtest_dep, zen_dep ]
)

