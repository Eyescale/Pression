[TOC]

# Introduction

Welcome to Pression, a C++ library for implementing and loading
compression and CPU-GPU data transfer plugins.

The
[API documentation](http://eyescale.github.io/Pression-1.1/index.html) can be
found on [eyescale.github.io](http://eyescale.github.io/). As with any open
source project, the available source code, in particular the
[unit tests](https://github.com/Eyescale/Pression/tree/1.1/tests) provide a
reference for developing applications.

Technical questions can be posted to the Equalizer
[Developer Mailing List](http://www.equalizergraphics.com/cgi-bin/mailman/listinfo/eq-dev),
or directly to
[info@equalizergraphics.com](mailto:info@equalizergraphics.com?subject=Pression%20question).

Commercial support, custom software development and porting services are
available from [Eyescale](http://www.eyescale.ch). Please contact
[info@eyescale.ch](mailto:info@eyescale.ch?subject=Pression%20support)
for further information.

# Features

Pression provides the following major features:

* Compression and CPU-GPU transfer plugin handling: pression::Compressor,
  pression::Decompressor, pression::Downloader, pression::Plugin,
  pression::PluginRegistry, pression::Uploader
* Detailed @ref Changelog

# Building

Pression is a cross-platform library, designed to run on any modern
operating system, including all Unix variants and the Windows operating
system. Pression uses CMake to create a platform-specific build
environment. The following platforms and build environments are tested:

* Linux: Ubuntu 14.04, RHEL 6.6 (Makefile, Ninja)
* Windows: 7 (Visual Studio 2012)
* Mac OS X: 10.8 (Makefile, Ninja)

Building from source is as simple as:


    git clone https://github.com/Eyescale/Pression.git
    mkdir Pression/build
    cd Pression/build
    cmake ..
    make

# FAQ

## On what basis where the compression parameters determined?

They are relative numbers when compared to the RLE compressor and the
data compressed. Since both change over time with new compilers, CPUs
and tested data, they are a somewhat arbitrary snapshot. Typically, all
compressor parameters are updated when a new plugin is added on a
representative visualization cluster node at the time. They are
determined by running the tests/compressor benchmark which calculates
them.
