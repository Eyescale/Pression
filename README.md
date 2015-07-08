[TOC]

# Introduction {#Introduction}

Welcome to Pression, a C++ library for implementing and loading
compression and CPU-GPU data transfer plugins

The
[API documentation](http://eyescale.github.io/Pression-1.0/index.html) can be
found on [eyescale.github.io](http://eyescale.github.io/). As with any open
source project, the available source code, in particular the
[unit tests](https://github.com/Eyescale/Pression/tree/1.0/tests) provide a
reference for developing applications.

Technical questions can be posted to the Equalizer
[Developer Mailing List](http://www.equalizergraphics.com/cgi-bin/mailman/listinfo/eq-dev),
or directly to
[info@equalizergraphics.com](mailto:info@equalizergraphics.com?subject=Pression%20question).

Commercial support, custom software development and porting services are
available from [Eyescale](http://www.eyescale.ch). Please contact
[info@eyescale.ch](mailto:info@eyescale.ch?subject=Pression%20support)
for further information.

# Building {#Building}

Pression is a cross-platform library, designed to run on any modern
operating system, including all Unix variants and the Windows operating
system. Pression uses CMake to create a platform-specific build
environment. The following platforms and build environments are tested:

* Linux: Ubuntu 14.04, RHEL 6.6 (Makefile, Ninja)
* Windows: 7 (Visual Studio 2012)
* Mac OS X: 10.8 (Makefile, Ninja)

~~~
git clone https://github.com/Eyescale/Pression.git
cd Pression
mkdir build
cd build
cmake ..
make
~~~

# Changes {#Changes}

## Version 1.0 {#Version10}

* Initial release as a separate project from Lunchbox
