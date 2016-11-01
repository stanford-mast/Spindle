libspindle
==========

libspindle is a multi-platform topology-aware thread control library for x86-based systems containing one or more NUMA nodes.
It features a simple API for dispatching tasks to multiple threads, organizing tasks into groups, and affinitizing tasks to logical cores.

Synchronization between threads is provided by means of thread barriers, both locally (within a task group) and globally (across all spawned threads).
The barrier implementation is designed to be high performance, leveraging the hardware cache coherency protocol to reduce overhead and virtually eliminate cache line thrashing.
A thread waiting at a barrier spins on a shared cache line that is not written until the last thread passes the barrier.
This write-once behavior keeps the shared cache line in `S` state in the caches of all cores while they are waiting at the barrier.

libspindle is implemented using a combination of C and assembly.


Requirements
============

Compatibility has been verified with 64-bit versions of Windows 10 Pro and Ubuntu 14.04.
Other Linux distributions are also likely compatible, but they have not been tested.

Some features of libspindle make use of AVX instructions introduced in processors of the Sandy Bridge generation.
Do not attempt to use it on older-generation processors, at the risk of encountering "Illegal Instruction" errors.

To obtain system topology information in a platform-independent manner, libspindle depends on the [**hwloc**](https://www.open-mpi.org/projects/hwloc/) library and has been tested with version 1.11.4.
Some distributions of Linux may make this library available as a package.
Otherwise, and in all cases on Windows, it must be downloaded and installed manually.


Compiling
=========

On all platforms, libspindle compiles to a static library.

The Windows build system is based on Visual Studio 2015 Community Edition. Compilation is known to work from the graphical interface, but command-line build is also likely possible.

To build on Linux, just type `make` from within the repository directory.


Using
=====

Documentation is available and can be built using Doxygen.

On Linux, type `make docs` to compile the documentation. On Windows, run the Doxygen tool using the repository directory as the working directory.

Projects that make use of libspindle should include the top-level spindle.h header file and nothing else.
Documentation covers both spindle.h and the internal implementation of libspindle, the latter being of interest only to those who wish to modify the implementation of libspindle.

Linking with the libspindle static library will also require linking with hwloc and its dependencies.
On Windows it is recommended to compile the `libhwloc` project as a static library; it would then be sufficient to add both `spindle.lib` and `libhwloc-5.lib` as additional library dependencies to any Visual Studio project that uses libspindle.

At a low level, libspindle globally reserves and assumes exclusive use of the `ymm15` AVX register. It uses this register to hold thread information.
Code that executes in regions parallelized by libspindle must not modify the contents of this register, even inadvertently by means of the `vzeroupper` instruction.
On Linux, projects that use libspindle and make use of vector instructions (SSE, AVX, and so on) should be compiled with the GCC option `-mno-vzeroupper`.


Examples
========

Examples coming soon.

~~~{.c}
int main(int argc, char* argv[])
{
    // This is just a placeholder.
    return 0;
}
~~~


Copyright
=========

libspindle is licensed under BSD 3-clause (see "LICENSE" in the top-level source code directory).

Copyright (c) 2016 Stanford University, Department of Electrical Engineering.
Authored by Samuel Grossman.
