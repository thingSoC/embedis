# Embedis - Embedded Dictionary Server [![Build Status](https://travis-ci.org/thingSoC/embedis.png?branch=master)](https://travis-ci.org/thingSoC/embedis)

This README is for anyone wanting to develop the Embedis server itself.
If you just want to use the Embedis server, begin with the [main documentation]
(http://thingSoC.github.io/embedis).

## Development Environment

The git repository includes a submodule for the testing framework and a
branch 'gh-pages' for the generated documentation. Here's the recommended
commands to use:

```
git clone {origin_url} embedis
cd embedis
git submodule init
git submodule update
git clone {origin_url} html --branch gh-pages
```

You'll need a few tools to build the tests and documentation. These are
common tools available for all operating systems so you shouldn't have
too much trouble getting them installed.

 * [CMake](http://www.cmake.org)
 * [Doxygen](http://www.doxygen.org)

CMake can create a typical Makefile as well as project files for Xcode,
Visual Studio, and many others. Here's a quick start for Makefile users:

```
$ cmake .
$ make && ctest
$ make doc
```
