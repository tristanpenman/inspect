# Inspect

## Overview

Inspect is a _very_ simple spreadsheet application.

This project is essentially experimental code - I wrote it while learning how to use Ragel and Lemon to build a parser. The build system was hacked together pretty quickly so that it would work on Mac. It should compile cleanly on Linux systems, but I have not tested this recently.

## Dependencies

Build dependencies (these can be installed using a package manager such as 'apt' or 'brew'):

- [Ragel state machine compiler](http://www.complang.org/ragel/)
- [Lemon parser generator](https://www.sqlite.org/lemon.html)

Vendored:

- [googletest](https://code.google.com/p/googletest/) (for unit tests only, included in 'vendor' directory)

## Build

Inspect can be built using CMake. The recommended approach is to create a build directory, and to run 'cmake' from there. For example:

    mkdir build
    cd build
    cmake ..
    make

For Windows... you're on your own.

## Usage

This project includes a miniature REPL (`inspect_console`) for interacting with a _sheet_. A sheet is really just a collection of cells that can be identified using typical spreadsheet addresses (e.g. A1, B2, C12, etc).

If you followed the build instructions above, you can start the REPL using:

    ./inspect_console

You'll be greeted with a `>` prompt, where you can assign formulas to cells. After each assignment, the current state of the sheet will be printed out, e.g.:

    > A1 = 1
    [1,1]: 1
    > A2 = A1
    [1,1]: 1
    [1,2]: 1
    > B3 = A2 * 2
    [1,1]: 1
    [1,2]: 1
    [2,3]: 2
    > B4 = B3 + A1 * 2
    [1,1]: 1
    [1,2]: 1
    [2,3]: 2
    [2,4]: 4
    >

You can force the parser to treat a formula as a literal value, using an apostrophe `'` instead of equals `=`:

    > A1 'This is my literal
    [1,1]: This is my literal
    >

And you can inspect the formula and value of an individual cell:

    > A1 = 2
    [1,1]: 2
    > A2 = A1 + 1
    [1,1]: 2
    [1,2]: 3
    > A2
    A2 = A1 + 1 = 3
    >

The REPL will tell you if your input is invalid:

    > Some invalid input
    Error: Invalid input.
    >

## Project structure

      * etc          Contains lemon parser template
      * src          Source files
      * test         Test source files
      * vendor       Third party library dependencies

## License

Inspect is licensed under the Simplified BSD License. See the LICENSE file for more information.
