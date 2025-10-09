project('oshell', 'c',
  version: '0.0.1',
  default_options: [
    'c_std=c11',
    'warning_level=3',
    # 'optimization=3', # this conflicts with buildtype=debug
    'buildtype=debug'
  ]
)

# Add pedantic warnings for GCC/Clang
if meson.get_compiler('c').get_id() == 'gcc' or meson.get_compiler('c').get_id() == 'clang'
  add_project_arguments('-Wpedantic', language: 'c')
endif

# Include directories
inc_dir = include_directories('include')

# Define source files
sources = [
  'src/main.c',
  'src/utils.c',
  'src/parsing.c',
  'src/parsing_utils.c',
  'src/execute_command.c',
  # 'src/input_redirection.c',
  'src/piping.c',
  'src/clear.c',
  'src/kill_process.c',
  'src/chaining.c'
]

# Create executable
executable('oshell', sources, 
  include_directories: inc_dir,
  install: false
)
