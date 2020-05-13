test_src += files(['test.cpp'])

comp_n_run = executable('test.x',['test.cpp']+headers,
    include_directories: include_dirs,
    link_with: g2_lib)
test('compile and run',comp_n_run)
