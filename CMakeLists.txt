# Address Sanitizer
# cmake -DENABLE_ASAN=ON ..

# Memory Sanitizer
# cmake -DENABLE_MSAN=ON ..

# Undefined Behavior Sanitizer
# cmake -DENABLE_UBSAN=ON ..

# Thread Sanitizer
# cmake -DENABLE_TSAN=ON ..

# Memory Sanitizer (MSAN) only works with Clang
# You typically can't combine MSAN with other sanitizers
# Thread Sanitizer (TSAN) can't be combined with Address Sanitizer (ASAN)

cmake_minimum_required(VERSION 3.10)
project(oshell VERSION 0.0.1 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set warning level and optimization
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

# Debug build type by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Sanitizer options
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_MSAN "Enable Memory Sanitizer" OFF)

# Configure sanitizers
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

if(ENABLE_TSAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

if(ENABLE_MSAN)
    add_compile_options(-fsanitize=memory -fno-omit-frame-pointer)
    add_link_options(-fsanitize=memory)
endif()

# Include directories
include_directories(include)

# Define source files
set(SOURCES
    src/main.c
    src/utils.c
    src/parsing/parsing.c
    src/parsing/execute_command.c
    src/parsing/input_redirection.c
    src/parsing/piping.c
    src/parsing/clear.c
)

# Create executable
add_executable(oshell ${SOURCES})
