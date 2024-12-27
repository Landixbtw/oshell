project('oshell', 'c', version: '0.0.1',
default_options: [
    'warning_level=3',
    'debug=true',
    'optimization=3',
    'c_std=gnu11',
])

inc_dir = include_directories('include')


executable(
   'oshell', 
['src/main.c', 
    'src/utils.c',
    'src/parsing/parsing.c', 'src/parsing/execute_command.c', 'src/parsing/validate_command.c'

  ],
include_directories: inc_dir,
build_by_default: true,
)
