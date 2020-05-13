project('gadget2-reader','cpp',
        default_options : ['cpp_std=c++17',
       'warning_level=3' ],
        version: '0.0.1')

subdir('include')
subdir('src')

_format = find_program('clang-format', required:false)
pkg=import('pkgconfig')


g2_lib = library('gadget2',src+headers,include_directories: include_dirs,
    install: true)

g2_exe = executable('gadget2-ls',g2_src+headers,include_directories: include_dirs,
    install: true,
    link_with: g2_lib)

install_headers(headers)

pkg.generate(g2_lib,
    description: 'Library for reading Gadget files')

subdir('tests')

if (_format.found())
  run_target('format', command: [_format, '-i', headers,src,g2_src,test_src])
endif
