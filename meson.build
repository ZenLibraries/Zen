
project('zen2', 'cpp', version: '0.0.1')

debug = get_option('debug')
zen_enable_tests = get_option('zen_enable_tests')
zen_enable_assertions = get_option('zen_enable_assertions')

if zen_enable_tests
  gtest_proj = subproject('gtest')
  gtest_dep = gtest_proj.get_variable('gtest_main_dep')
endif

zen_namespace = get_option('zen_namespace')
zen_namespace_start = ''
zen_namespace_end = ''

foreach name : zen_namespace.split('::')
  zen_namespace_start += 'namespace ' + name + ' {'
  zen_namespace_end += '}'
endforeach

zen_compile_args = [
  '-std=c++17',
  '-DZEN_NAMESPACE=' + zen_namespace,
  '-DZEN_NAMESPACE_START=' + zen_namespace_start,
  '-DZEN_NAMESPACE_END=' + zen_namespace_end,
]

if debug
  zen_compile_args += [ '-fstandalone-debug' ]
endif

if zen_enable_assertions
  zen_compile_args += [ '-DZEN_ENABLE_ASSERTIONS=1' ]
else
  zen_compile_args += [ '-DZEN_ENABLE_ASSERTIONS=0' ]
endif

zen_lib = static_library(
  'zen',
  'src/program_options.cc',
  include_directories: 'include',
  cpp_args: zen_compile_args,
)

zen_dep = declare_dependency(
  include_directories: 'include',
  compile_args: zen_compile_args,
  link_with: [ zen_lib ],
)

if zen_enable_tests
  executable(
    'alltests',
    'test/iterator_adaptor.cc',
    'test/zip_iterator.cc',
    'test/program_options.cc',
    include_directories: 'include',
    dependencies: [ gtest_dep, zen_dep ],
    build_by_default: false
  )
endif

