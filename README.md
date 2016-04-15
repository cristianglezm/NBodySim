# NBodySim #

N-Body Simulation using a Barnes Hut.

## Building ##

Requires CMake and one of the following compilers:

    GCC 4.8 or newer;
    Clang 3.7 or newer [Not Tested];
    Visual Studio 2010 or newer [Not Tested];

NBodySim requires the following dependencies:

* [SFML](http://sfml-dev.org) 2.3 or newer [Not Tested]

Use CMake to build the project and install.

```
mkdir build && cd build
cmake -DSFML_ROOT=<sfml root> ..
make -j3
make install
```