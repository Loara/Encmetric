# Encmetric
![GitHub release (latest by date)](https://img.shields.io/github/v/release/Loara/Encmetric?color=brightgreen)
![GitHub](https://img.shields.io/github/license/Loara/Encmetric?color=blue&label=License&style=plastic)
![GitHub top language](https://img.shields.io/github/languages/top/Loara/Encmetric?color=blue)
[![GitHub issues](https://img.shields.io/github/issues/Loara/Encmetric)](https://github.com/Loara/Encmetric/issues)

C++ library to manage different encoding strings

# License
Encmetric is written under the GNU Lesser General Public License (LGPL) version 3.0. For more informations see COPYING and COPYING.LESSER files

# Build and Install
To build the library you need cmake version 3.19 or later and a C++17 compiler.

To build and install the library

    cmake src/
    cmake --build .
    cmake --install .

to build also the test executable turn on the option ```BUILD_TEST``` in cmake.

# Encodings currently included in this library (v. 1.1)
* ASCII
* Latin1 / ISO8859-1
* ISO8859-2
* UTF8
* UTF16LE and UTF16BE
* UTF32LE and UTF32BE

Clearly you can write and use your own encoding classes.
