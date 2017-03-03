Spindle is a multi-platform topology-aware thread control library for x86-based systems containing one or more NUMA nodes.
It features a simple API for dispatching tasks to multiple threads and affinitizing threads to logical cores.

Synchronization between threads is provided by means of thread barriers, both locally (within a task) and globally (across all spawned threads).
The barrier implementation is designed to be high performance, leveraging the hardware cache coherency protocol to reduce overhead and virtually eliminate cache line thrashing.
A thread waiting at a barrier spins on a shared cache line that is not written until the last thread passes the barrier.
This write-once behavior keeps the shared cache line in `S` state in the caches of all cores while they are waiting at the barrier.

Spindle is implemented using a combination of C and assembly.


# Requirements

To build and link with Spindle, the following are required.

- 64-bit x86-compatible processor with support for AVX instructions
  
  Spindle has been tested with Intel processors of the Sandy Bridge generation and newer.
  Non-Intel processors may also be compatible if they support the required instructions.
  Do not attempt to use it on older-generation processors, at the risk of encountering "Illegal Instruction" errors.
  
- Windows 10 Pro or Ubuntu 14.04
  
  64-bit versions of these operating systems are required.
  Other Linux distributions are likely compatible, but they have not been tested.

- **Topo**
  
  Topo provides some helpers and wrappers around the `hwloc` library.

- [**hwloc**](https://www.open-mpi.org/projects/hwloc/) and its dependencies
  
  Spindle makes use of this library to obtain system topology information in a platform-independent manner.
  It has been tested with version 1.11.4.
  Some distributions of Linux may make this library available as a package.
  Otherwise, and in all cases on Windows, it must be downloaded and installed manually.


# Building

On all platforms, Spindle compiles to a static library.

The Windows build system is based on Visual Studio 2015 Community Edition. Compilation is known to work from the graphical interface, but command-line build is also likely possible.

To build on Linux, just type `make` from within the repository directory.


# Linking and Using

Projects that make use of Spindle should include the top-level spindle.h header file and nothing else.

At a low level, Spindle globally reserves and assumes exclusive use of the `ymm15` AVX register. It uses this register to hold thread information.
Code that executes in regions parallelized by Spindle must not modify the contents of this register, even inadvertently by means of the `vzeroupper` instruction.
On Linux, projects that use Spindle and make use of vector instructions (SSE, AVX, and so on) should be compiled with the GCC option `-mno-vzeroupper`.

Assuming a Linux-based C-language project that uses Spindle and consists of a single source file called `main.c`, the following command would build and link with Spindle.

    gcc main.c -mno-vzeroupper -pthread -lspindle -ltopo -lhwloc -lnuma -lpciaccess -lxml2


# Getting Started

Documentation is available and can be built using Doxygen.
It covers both the external API in spindle.h and Spindle's internals, the latter being of interest only to those who wish to modify the implementation of Spindle.

On Linux, type `make docs` to compile the documentation. On Windows, run the Doxygen tool using the repository directory as the working directory (it may be necessary to create the output directory manually first).

The remainder of this section is designed to facilitate a quick understanding of the Spindle API.
It explains key concepts and defines key terms, providing pointers to relevant API functions and data structures.
Source code examples are provided immediately following.


## Concepts and Terminology

The core unit of execution in Spindle is a _task_: a function that Spindle executes using one or more threads.
Each task is specified to Spindle as a _task specification_, which specifies to Spindle what function to call, what argument to pass (the same for all threads in a task), and some way to determine both the number of threads to create and how to affinitize them to the system's logical cores.

Every thread spawned by Spindle is associated with three identifiers: a _local ID_, a _global ID_, and a _task ID_.
A thread's local ID is unique only within its task, whereas a thread's global ID is unique across all threads spawned.
Both such IDs take the form of indices ranging from 0 to one less than the number of threads created, either locally or globally, respectively.
Task IDs are defined similarly, ranging from 0 to one less than the number of tasks created.

All threads within a given task must be physically affinitized to logical cores on the same NUMA node.
Parallelizing a task across NUMA nodes is supported by creating multiple tasks, one for each desired NUMA node, with the same function and argument.
This facilitates implementation of tasks that are NUMA-aware; threads can determine their logical NUMA node assignment based on their task ID.

Once spawned, threads can access their respective IDs using spindleGetLocalThreadID(), spindleGetGlobalThreadID(), and spindleGetTaskID().
Threads can also access information about the number of threads spawned using spindleGetLocalThreadCount(), spindleGetGlobalThreadCount(), and spindleGetTaskCount().
These functions should not be called outside of threads spawned by Spindle.

Threads are spawned by calling spindleThreadsSpawn() and passing as parameters a pointer to an array of task specifications and the number of entries in the array.
Each task specification takes the form of an instance of #SSpindleTaskSpec.
This function blocks until all threads spawned have exited, after which it returns to the caller.
The calling thread is blocked; only spawned threads execute the tasks.

Synchronization in Spindle is provided by means of _thread barriers_, which prevent threads from passing the point of the barrier (in program order) until all threads have reached the barrier.
Two types of barriers are provided: spindleBarrierLocal() implements a thread barrier only with respect to other threads in the same task, and spindleBarrierGlobal() implements a thread barrier across all spawned threads.
If it is of interest to measure the amount of time spent waiting at a barrier, spindleTimedBarrierLocal() and spindleTimedBarrierGlobal() are both available.
These variations measure, using the `rdtsc` instruction, the number of cycles spent waiting at the barrier and return the result.

As a convenience, Spindle provides each thread with a 64-bit per-thread local variable, which can be used for any purpose and is initialized to 0 each time threads are spawned.
Its value can be accessed using spindleGetLocalVariable() and updated using spindleSetLocalVariable().
This variable is stored in part of the register that Spindle reserves, so accesses and updates are extremely efficient.


## Thread Assignment

Spindle assigns threads to logical cores during the thread spawning process.
A _logical core_ corresponds to a hardware thread and is the smallest available unit of thread affinitization.
Each _physical core_ in the system contains one or more logical cores, depending on whether or not the system supports SMT (simultaneous multithreading).
Note that Spindle will never assign threads for different tasks to the same physical core.

Every Spindle task specification includes an SMT policy, which tells Spindle how it should assign threads to logical cores in the system.
See #ESpindleSMTPolicy for more details on supported SMT policies and associated behaviors.

Spindle's thread assignment process follows these two steps.
1. Compute the number of physical cores needed to accomodate all threads in a task. If SMT is disabled per the SMT policy, this is equal to the number of threads. Otherwise it is computed by taking into account the number of logical cores per physical core.
2. Assign one thread to each logical core, in the order specified by the SMT policy.


## Examples

Source code examples are organized into two categories: task specification and task implementation.
Each category of examples showcases different parts of Spindle's API.


### Specifying Tasks

All examples in this category assume the existance of an array, `taskFuncs`, of pointers to task functions.

#### Example 1: Simple Task Specification

This example shows how to specify tasks manually.
Two tasks are created on the same NUMA node, one which requires 2 threads and one which uses all remaining logical cores, the number of threads being determined automatically.

~~~{.c}
#include <spindle.h>

int main(int argc, char* argv[])
{
    SSpindleTaskSpec task[2];
    
    task[0].arg = NULL;
    task[0].func = taskFuncs[0];
    task[0].numaNode = 0; // First NUMA node.
    task[0].numThreads = 2; // Create exactly 2 threads.
    task[0].smtPolicy = SpindleSMTPolicyPreferLogical;
    
    task[1].arg = NULL;
    task[1].func = taskFuncs[1];
    task[1].numaNode = 0; // First NUMA node.
    task[1].numThreads = 0; // Use all remaining available logical cores.
    task[1].smtPolicy = SpindleSMTPolicyPreferLogical;
    
    // Spawn the threads, using the calling thread as one of the workers.
    spindleThreadsSpawn(task, 2, true);
    
    return 0;
}
~~~

#### Example 2: Single NUMA-Aware Task

This example shows how to run a single task on multiple NUMA nodes.
Each thread's task ID corresponds to a logical identifier for the NUMA node on which it is executing.

~~~{.c}
#include <spindle.h>
#include <topo.h>

int main(int argc, char* argv[])
{
    // Use a Spindle-provided helper to figure out the number of NUMA nodes in the system.
    const unsigned int numNumaNodes = topoGetSystemNUMANodeCount();
    
    SSpindleTaskSpec* task = (SSpindleTaskSpec*)malloc(sizeof(SSpindleTaskSpec) * numNumaNodes);
    if (NULL == task)
        return 1;
    
    // Define the tasks iteratively.
    for (unsigned int i = 0; i < numNumaNodes; ++i)
    {
        task[i].arg = NULL;
        task[i].func = taskFuncs[0]; // Run the same task function on all NUMA nodes.
        task[i].numaNode = i; // Different NUMA node per task.
        task[i].numThreads = 0; // Use all available logical cores on each NUMA node.
        task[i].smtPolicy = SpindleSMTPolicyPreferLogical;
    }
    
    // Spawn the threads, using the calling thread as one of the workers.
    spindleThreadsSpawn(task, numNumaNodes, true);
    
    free((void*)task);
    return 0;
}
~~~


### Implementing Tasks

Task implementations consist of functions that are invoked as part of Spindle's thread spawning process.
Within the body of a task function, many of Spindle's API functions become available for use, including thread identification and synchronization barriers.

#### Example 1: Simple Task Implementation

This example shows a very simple task function that simply identifies each thread.
Note that the function signature matches the type definition for #TSpindleFunc.

The local barrier ensures that, within each task, all "Before" messages will be printed before any "After" messages.
Switching to a global barrier would cause all "Before" messages to occur before any "After" messages, irrespective of the task to which each thread belogs.

~~~{.c}
void taskFunc(void* arg)
{
    printf("Before %u (%u,%u)\n", spindleGetGlobalThreadID(), spindleGetTaskID(), spindleGetLocalThreadID());
    
    spindleBarrierLocal();
    
	printf("After %u (%u,%u)\n", spindleGetGlobalThreadID(), spindleGetTaskID(), spindleGetLocalThreadID());
}
~~~

#### Example 2: Synchronization

When using multiple tasks, potentially with different functions, global barriers can be used to synchronize their executions.
Note that each task must contain the same number of global barriers, otherwise some threads will become stuck waiting at barriers.

This example also shows how to ensure only certain threads within each task perform some actions.
Threads can simply use their own local IDs as a filter.
In this example, only the first thread in each task prints to standard output.

~~~{.c}
void threadFuncFirst(void* arg)
{
    if (0 == spindleGetLocalThreadID())
        puts("First");

    spindleBarrierGlobal();
}

void threadFuncSecond(void* arg)
{
    spindleBarrierGlobal();

    if (0 == spindleGetLocalThreadID())
        puts("Second");
}
~~~

The use of the global barrier in this manner ensures that "First" is always printed before "Second".
Omitting the global barrier in either one of `threadFuncFirst` or `threadFuncSecond` results in a program that does not terminate.


# Copyright

Spindle is licensed under BSD 3-clause (see "LICENSE" in the top-level source code directory).

Copyright (c) 2016-2017 Stanford University, Department of Electrical Engineering.
Authored by Samuel Grossman.
