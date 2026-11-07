cmake_minimum_required(VERSION 3.16)
project(inspect LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

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

# Inspect library
add_library(inspect
    ${CMAKE_CURRENT_BINARY_DIR}/generated/address.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/formula.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.c
    src/ast.cpp
    src/sheet.cpp
)

# Build local gtest
set(gtest_force_shared_crt ON)
set(BUILD_GMOCK OFF CACHE BOOL "Build GoogleMock" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Install GoogleTest" FORCE)
add_subdirectory(thirdparty/googletest)

target_include_directories(inspect
    PUBLIC src
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated
)

if(BUILD_TESTING)
    add_executable(inspect_tests
        test/address_test.cpp
        test/sheet_test.cpp
    )

    target_link_libraries(inspect_tests
        PRIVATE gtest gtest_main inspect
    )

    add_test(NAME inspect_tests COMMAND inspect_tests)
endif()

add_executable(inspect_console
    ${CMAKE_CURRENT_BINARY_DIR}/generated/console.cpp
)

target_link_libraries(inspect_console
    inspect
)
