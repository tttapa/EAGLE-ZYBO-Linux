# Structure

## Executables: `applications`

This folder contains implementation files and CMake files that build executable
files.

## Temporary build files, compiled libraries and executables: `build` & `build-arm`

These to folders are used by the build system to build everything.

## CMake scripts and Toolchain files: `cmake`

This folder contains the installation script for Google Test, and the Toolchain
file that defines the compile options for building code for the ZYBO.

## Utility Scripts: `scripts`

Currently, there is one script that copies all ARMv7 executables and libraries
from the Docker toolchain and OpenCV builds, and stores them in a filesystem in
a file that can be copied to the SD card. These libraries are needed to run the
compiled code.

## Documentation: `doc`

This folder contains documentation in the form of MarkDown documents, as well as
a recipe for automatically generated documentation using Doxygen.  
The output of Doxygen is in the `html` subfolder: use `index.html` as the entry
point.

## Docker build instructions: `docker`

This folder contains all files to build the Docker images with the toolchain and
necessary libraries.

## Source files: `src`

This folder contains the source code for the different libraries.  
The code is split up in "modules". Each of these modules has a specific task and
has a number of tests.

A module folder consists of three main folders and a CMake file:

- `src`  
  This folder contains all implementation files and private headers. They are
  necessary for building the library itself, but you don't need them when using
  the library.
- `include`  
  This folder contains all public headers. They define the public API of the 
  library, and are needed when using the library.
- `test`  
  This folder contains the unit tests for the library functions.
- `CMakeLists.txt`  
  This file defines how the library should be compiled.