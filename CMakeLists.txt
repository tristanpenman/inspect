cmake_minimum_required(VERSION 3.31)
project(inspect LANGUAGES C CXX)

# Force out-of-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# C++11 is required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid language extensions to ensure consistent behavior across compilers
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContent: Easier than Git submodules
include(FetchContent)

# Default to Debug for single-config generators if user didn't specify anything
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT isMultiConfig AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ----------------------------------------------------------------------------
# Generated sources
# ----------------------------------------------------------------------------

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/address.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/address.rl
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/console.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/console.rl
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
)

add_custom_command(
    COMMAND ragel ${CMAKE_CURRENT_SOURCE_DIR}/src/formula.rl -o ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/formula.rl
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
)

# Copy parser inputs into the build tree using CMake's portable file handling.
configure_file(src/parser.y generated/parser.y COPYONLY)
configure_file(src/parser.h generated/parser.h COPYONLY)
configure_file(etc/lempar.c generated/lempar.c COPYONLY)

# Generate a parser from parser.y
add_custom_command(
    COMMAND lemon -m -q ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.y ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.h
        ${CMAKE_CURRENT_BINARY_DIR}/generated/lempar.c
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.c
)

# ----------------------------------------------------------------------------
# Inspect library
# ----------------------------------------------------------------------------

add_library(inspect
    ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.c
    src/ast.cpp
    src/sheet.cpp
)

target_include_directories(inspect PUBLIC
    src
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# ----------------------------------------------------------------------------
# Console executable
# ----------------------------------------------------------------------------

add_executable(inspect_console
    ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
)

target_link_libraries(inspect_console PRIVATE inspect)

# ----------------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------------

include(CTest)
if(BUILD_TESTING)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(inspect_tests
        test/address_test.cpp
        test/sheet_test.cpp
    )

    target_link_libraries(inspect_tests PRIVATE
        GTest::gtest_main
        inspect
    )

    add_test(NAME inspect_tests COMMAND inspect_tests)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR} (multi-config: ${isMultiConfig})")
