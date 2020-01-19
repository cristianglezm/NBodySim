# NBodySim #

N-Body Simulation using a Barnes Hut.

Keys:

* F -> Fast generation press left mouse button to add bodies.
* T -> To see the Barnes-Hut regions.
* B -> To add a body fixed to its position with a mass 100 times current mass.
* Left Shift -> Increases mass.
* Left Control -> Decreases mass.
* Arrow Keys -> Move the camera.
* \+ key -> Zoom in.
* \- key -> Zoom out.

## Building ##

Requires CMake and one of the following compilers:

    GCC 8.1 or newer;
    Clang 3.7 or newer [Not Tested];
    Visual Studio 2010 or newer [Not Tested];

NBodySim requires the following dependencies:

* [SFML](http://sfml-dev.org) 2.5.1 or newer

Use CMake to build the project and install.

```
mkdir build && cd build
cmake -DSFML_ROOT=<sfml root> ..
make -j3
make install
```
