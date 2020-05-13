test_src += files(['test.cpp'])

osnapshot = executable('test.x',['test.cpp']+headers,
    include_directories: include_dirs,
    link_with: g2_lib)
test('osnapshot',osnapshot)
