test_env = [
  'GIO_USE_VOLUME_MONITOR=unix',
  'GSETTINGS_BACKEND=memory',
  'G_ENABLE_DIAGNOSTIC=0',
  'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
  'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
]

tests = [
  'coordinate',
  'file-cache',
  'license',
  'marker',
  'map',
  'marker-layer',
  'memory-cache',
  'network-tile-source',
  'viewport',
]

if get_option('vector_renderer')
  tests += [
    'vector-expression',
    'vector-style',
    'vector-value',
  ]
endif

subdir('data')

test_utils_sources = [
  'test-tile-server.c',
]
testutils_lib = shared_library(
  'testutils',
  test_utils_sources,
  dependencies: libshumate_dep,
)
testutils_dep = declare_dependency(
  link_with: testutils_lib,
  dependencies: libshumate_deps,
  sources: test_utils_sources,
  include_directories: include_directories('../shumate')
)

foreach test : tests
  executable = executable(
    test,
    test_resources,
    '@0@.c'.format(test),
    dependencies: [libshumate_dep, testutils_dep],
  )

  test(test, executable, env: test_env)
endforeach
