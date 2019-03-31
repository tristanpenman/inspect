cmake_minimum_required (VERSION 2.6)
project (inspect)

include(FindPkgConfig)

# Build local gtest
add_subdirectory(vendor/gtest-1.7.0)

# Include path
include_directories(
    src
    vendor/gtest-1.7.0/include
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/address.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
    DEPENDS src/address.rl ${CMAKE_CURRENT_BINARY_DIR}/generated
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/console.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
    DEPENDS src/console.rl ${CMAKE_CURRENT_BINARY_DIR}/generated
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/formula.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
    DEPENDS src/formula.rl ${CMAKE_CURRENT_BINARY_DIR}/generated
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
)

# Copy parser.y to build directory
add_custom_command(
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/generated
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y
)

# Copy parser.h to build directory
add_custom_command(
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.h ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.h ${CMAKE_CURRENT_BINARY_DIR}/generated
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.h
)

# Copy lempar.c template into build directory
add_custom_command(
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/etc/lempar.c ${CMAKE_CURRENT_BINARY_DIR}/generated/lempar.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/etc/lempar.c
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/lempar.c
)

# Generate a parser from parser.y
add_custom_command(
    COMMAND lemon -m -q ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/lempar.c
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.c
)

# Inspect library
add_library(inspect
    ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.c
    src/ast.cpp
    src/sheet.cpp
)

# Unit tests executable
add_executable(inspect_tests
    test/address_test.cpp
    test/sheet_test.cpp
)

target_link_libraries(inspect_tests
    gtest
    gtest_main
    inspect
)

add_executable(inspect_console
    ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
)

target_link_libraries(inspect_console
    inspect
)
