/*****************************************************************************
 * Spindle
 *   Multi-platform topology-aware thread control library.
 *   Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file spawn.c
 *   Implementation of all thread-spawning logic.
 *****************************************************************************/

#include "../spindle.h"
#include "barrier.h"
#include "osthread.h"
#include "types.h"

#include <hwloc.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <topo.h>


// -------- HELPERS -------------------------------------------------------- //

/// Retrieves the `hwloc` processing unit object to which the specified thread should be affinitized.
/// Parameters specify the `hwloc` system topology, the task definition, and the SMT policy, all of which are used to identify the processing unit.
/// Performs minimal, if any, error-checking and assumes a correct assignment of physical cores to tasks.
/// @param [in] topology System topology object, from `hwloc`.
/// @param [in] startPhysCore Starting physical core, part of the task specification.
/// @param [in] endPhysCore Ending physical core, part of the task specification.
/// @param [in] threadIndex Zero-based index of the thread within the task (in other words, the thread's local ID).
/// @param [in] smtPolicy SMT policy, part of the task specification.
/// @return Object representing the `hwloc` processing unit to which the specified thread should be affinitized.
static hwloc_obj_t spindleHelperGetThreadAffinityObject(hwloc_topology_t topology, uint32_t startPhysCore, uint32_t endPhysCore, uint32_t threadIndex, ESpindleSMTPolicy smtPolicy)
{
    const uint32_t numPhysCoresAvailable = endPhysCore - startPhysCore + 1;
    hwloc_obj_t affinityObject = NULL;
    
    switch (smtPolicy)
    {
    case SpindleSMTPolicyDisableSMT:
        // Each thread consumes a whole physical core, so verify that the index is within bounds.
        if (threadIndex < numPhysCoresAvailable)
        {
            // First, get the physical core at the specified index.
            hwloc_obj_t physicalCoreObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, startPhysCore + threadIndex);
            
            // Next, get the first logical core of the corresponding physical core.
            if (NULL != physicalCoreObject)
                affinityObject = hwloc_get_obj_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU, 0);
        }
        break;

    case SpindleSMTPolicyPreferPhysical:
        if (1)
        {
            // Calculate physical and logical core indices.
            // The current implementation assumes all physical cores per task have the same number of logical cores.
            const uint32_t physicalCoreIndex = startPhysCore + (threadIndex % numPhysCoresAvailable);
            const uint32_t logicalCoreIndex = threadIndex / numPhysCoresAvailable;

            // Obtain the correct physical core.
            hwloc_obj_t physicalCoreObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, physicalCoreIndex);

            // Obtain the correct logical core as the affinity object.
            if (NULL != physicalCoreObject)
                affinityObject = hwloc_get_obj_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU, logicalCoreIndex);
        }
        break;

    case SpindleSMTPolicyPreferLogical:
        if (1)
        {
            // First, get the physical core at the starting physical core index and, from this, obtain the number of logical cores per physical core.
            // The current implementation assumes all physical cores per task have the same number of logical cores.
            hwloc_obj_t physicalCoreObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, startPhysCore);
            if (NULL == physicalCoreObject)
                break;

            const uint32_t logicalCoresPerPhysicalCore = hwloc_get_nbobjs_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU);

            // Next, calculate physical and logical core indices.
            const uint32_t physicalCoreIndex = startPhysCore + (threadIndex / logicalCoresPerPhysicalCore);
            const uint32_t logicalCoreIndex = threadIndex % logicalCoresPerPhysicalCore;

            // Obtain the correct physical core.
            physicalCoreObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, physicalCoreIndex);

            // Obtain the correct logical core as the affinity object.
            if (NULL != physicalCoreObject)
                affinityObject = hwloc_get_obj_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU, logicalCoreIndex);
        }
        break;

    default:
        break;
    }
    
    return affinityObject;
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "spindle.h" for documentation.

uint32_t spindleThreadsSpawn(SSpindleTaskSpec* taskSpec, uint32_t taskCount, bool useCurrentThread)
{
    SSpindleThreadInfo* threadAssignments = NULL;
    uint32_t nextThreadAssignmentIndex = 0;
    uint32_t threadResult = 0;
    
    hwloc_topology_t topology;
    hwloc_obj_t numaNodeObject = NULL;
    hwloc_obj_t physicalCoreObject = NULL;

    uint32_t* taskStartPhysCore;
    uint32_t* taskEndPhysCore;
    uint32_t* taskNumThreads;
    
    uint32_t currentNumaNode = 0;
    uint32_t threadsLeftOnCurrentNumaNode = 0;
    uint32_t coresLeftOnCurrentNumaNode = 0;
    uint32_t numThreadsRequested = 0;
    uint32_t numNumaNodes = 0;
    uint32_t totalNumThreads = 0;
    
    // It is trivially a success case if the number of tasks is zero.
    if (0 == taskCount)
        return 0;
    
    // Obtain the hardware topology object for the current system.
    topology = topoGetSystemTopologyObject();
    if (NULL == topology)
        return __LINE__;
    
    // Figure out the highest possible NUMA node index, for error-checking purposes.
    numNumaNodes = topoGetSystemNUMANodeCount();
    if (1 > numNumaNodes)
        return __LINE__;
    
    // Initialize data structures to assign from the first NUMA node in the system.
    numaNodeObject = topoGetNUMANodeObjectAtIndex(currentNumaNode);
    if (NULL == numaNodeObject)
        return __LINE__;
    
    threadsLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_PU);
    coresLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE);
    
    physicalCoreObject = hwloc_get_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, 0);
    if (NULL == physicalCoreObject)
        return __LINE__;
    
    // Allocate memory for assignment arrays.
    taskStartPhysCore = (uint32_t*)malloc(sizeof(uint32_t) * taskCount);
    if (NULL == taskStartPhysCore)
        return __LINE__;

    taskEndPhysCore = (uint32_t*)malloc(sizeof(uint32_t) * taskCount);
    if (NULL == taskEndPhysCore)
    {
        free((void*)taskStartPhysCore);
        return __LINE__;
    }

    taskNumThreads = (uint32_t*)malloc(sizeof(uint32_t) * taskCount);
    if (NULL == taskNumThreads)
    {
        free((void*)taskStartPhysCore);
        free((void*)taskEndPhysCore);
        return __LINE__;
    }
    
    // Assign ranges of physical cores to tasks, based on the task specifications.
    for (uint32_t taskIndex = 0; taskIndex < taskCount; ++taskIndex)
    {
        // Verify the task specification's NUMA node.
        if (taskSpec[taskIndex].numaNode < currentNumaNode || taskSpec[taskIndex].numaNode >= numNumaNodes)
        {
            free((void*)taskStartPhysCore);
            free((void*)taskEndPhysCore);
            free((void*)taskNumThreads);
            return __LINE__;
        }
        
        // Reinitialize to a different NUMA node if the specified NUMA node is different.
        if (taskSpec[taskIndex].numaNode != currentNumaNode)
        {
            currentNumaNode = taskSpec[taskIndex].numaNode;
            
            numaNodeObject = topoGetNUMANodeObjectAtIndex(currentNumaNode);
            if (NULL == numaNodeObject)
            {
                free((void*)taskStartPhysCore);
                free((void*)taskEndPhysCore);
                free((void*)taskNumThreads);
                return __LINE__;
            }
            
            threadsLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_PU);
            coresLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE);
            
            physicalCoreObject = hwloc_get_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, 0);
            if (NULL == physicalCoreObject)
            {
                free((void*)taskStartPhysCore);
                free((void*)taskEndPhysCore);
                free((void*)taskNumThreads);
                return __LINE__;
            }
        }
        
        // Figure out the requested number of threads, based on any special constants passed.
        switch (taskSpec[taskIndex].numThreads)
        {
        case kSpindleTaskSpecThreadsSameAsPrevious:
            // Use the same number of threads as was ultimately used for the previous task.
            if (0 == taskIndex)
            {
                // Cannot assign same as previous number of threads if the current task is the first one specified.
                free((void*)taskStartPhysCore);
                free((void*)taskEndPhysCore);
                free((void*)taskNumThreads);
                return __LINE__;
            }
            
            numThreadsRequested = taskNumThreads[taskIndex - 1];
            break;
        
        default:
            // Use whatever number of threads as was specified directly in the input.
            numThreadsRequested = taskSpec[taskIndex].numThreads;
            break;
        }
        
        // Find the ending physical core for the current task, based on the number of threads specified.
        if (kSpindleTaskSpecAllAvailableThreads == numThreadsRequested)
        {
            // Verify that at least one core remains available on the current NUMA node.
            if (1 > coresLeftOnCurrentNumaNode)
            {
                free((void*)taskStartPhysCore);
                free((void*)taskEndPhysCore);
                free((void*)taskNumThreads);
                return __LINE__;
            }
            
            // Assign the starting physical core for the current task.
            taskStartPhysCore[taskIndex] = physicalCoreObject->logical_index;
            
            // Initialize the counter for the number of threads assigned to the present task.
            taskNumThreads[taskIndex] = 0;
            
            // Consume all the remaining physical cores on the present node.
            while (NULL != physicalCoreObject)
            {
                // Update the end physical core assignment.
                taskEndPhysCore[taskIndex] = physicalCoreObject->logical_index;

                // Update the number of threads assigned to the present task.
                if (SpindleSMTPolicyDisableSMT == taskSpec[taskIndex].smtPolicy)
                    taskNumThreads[taskIndex] += 1;
                else
                    taskNumThreads[taskIndex] += hwloc_get_nbobjs_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU);

                // Move to the next physical core.
                physicalCoreObject = hwloc_get_next_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, physicalCoreObject);
            }

            // Update the numbers of available cores and threads on the present NUMA node.
            coresLeftOnCurrentNumaNode = 0;
            threadsLeftOnCurrentNumaNode = 0;
        }
        else
        {
            uint32_t numThreadsAssignedForTask = 0;
            
            // Verify a sufficient number of cores and threads left on the current NUMA node.
            if (threadsLeftOnCurrentNumaNode < numThreadsRequested || (SpindleSMTPolicyDisableSMT == taskSpec[taskIndex].smtPolicy && coresLeftOnCurrentNumaNode < numThreadsRequested))
            {
                free((void*)taskStartPhysCore);
                free((void*)taskEndPhysCore);
                free((void*)taskNumThreads);
                return __LINE__;
            }
            
            // Assign the starting physical core for the current task.
            taskStartPhysCore[taskIndex] = physicalCoreObject->logical_index;

            // Specify the number of threads for the current task.
            taskNumThreads[taskIndex] = numThreadsRequested;
            
            // Assign one physical core at a time to the present task, 
            while (numThreadsAssignedForTask < numThreadsRequested)
            {
                // Calculate the number of threads consumed by the present physical core.
                const uint32_t numThreadsConsumed = hwloc_get_nbobjs_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU);

                // Check for errors: there needs to be a valid physical core object at this point.
                if (NULL == physicalCoreObject)
                {
                    free((void*)taskStartPhysCore);
                    free((void*)taskEndPhysCore);
                    free((void*)taskNumThreads);
                    return __LINE__;
                }
                
                // Update the end physical core assignment.
                taskEndPhysCore[taskIndex] = physicalCoreObject->logical_index;
                
                // Add to the total number of threads assigned to the present task.
                if (SpindleSMTPolicyDisableSMT == taskSpec[taskIndex].smtPolicy)
                    numThreadsAssignedForTask += 1;
                else
                    numThreadsAssignedForTask += numThreadsConsumed;

                // Deduct from the number of available cores and threads on the present NUMA node.
                coresLeftOnCurrentNumaNode -= 1;
                threadsLeftOnCurrentNumaNode -= numThreadsConsumed;

                // Move to the next physical core.
                physicalCoreObject = hwloc_get_next_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, physicalCoreObject);
            }
        }

        // Update the total number of threads created globally.
        totalNumThreads += taskNumThreads[taskIndex];
    }
    
    // Allocate memory for thread assignments.
    threadAssignments = (SSpindleThreadInfo*)malloc(sizeof(SSpindleThreadInfo) * totalNumThreads);
    if (NULL == threadAssignments)
    {
        free((void*)taskStartPhysCore);
        free((void*)taskEndPhysCore);
        free((void*)taskNumThreads);
        return __LINE__;
    }
    
    // Create thread information for each task.
    for (uint32_t taskIndex = 0; taskIndex < taskCount; ++taskIndex)
    {
        for (uint32_t threadIndex = 0; threadIndex < taskNumThreads[taskIndex]; ++threadIndex)
        {
            threadAssignments[nextThreadAssignmentIndex].func = taskSpec[taskIndex].func;
            threadAssignments[nextThreadAssignmentIndex].arg = taskSpec[taskIndex].arg;
            threadAssignments[nextThreadAssignmentIndex].topology = topology;
            threadAssignments[nextThreadAssignmentIndex].affinityObject = spindleHelperGetThreadAffinityObject(topology, taskStartPhysCore[taskIndex], taskEndPhysCore[taskIndex], threadIndex, taskSpec[taskIndex].smtPolicy);
            threadAssignments[nextThreadAssignmentIndex].localThreadID = threadIndex;
            threadAssignments[nextThreadAssignmentIndex].globalThreadID = nextThreadAssignmentIndex;
            threadAssignments[nextThreadAssignmentIndex].taskID = taskIndex;
            threadAssignments[nextThreadAssignmentIndex].localThreadCount = taskNumThreads[taskIndex];
            threadAssignments[nextThreadAssignmentIndex].globalThreadCount = totalNumThreads;
            threadAssignments[nextThreadAssignmentIndex].taskCount = taskCount;

            nextThreadAssignmentIndex += 1;
        }
    }
    
    // Allocate and initialize all thread barrier memory regions.
    spindleInitializeGlobalThreadBarrier(totalNumThreads);
    
    if (NULL == spindleAllocateLocalThreadBarriers(taskCount))
    {
        free((void*)taskStartPhysCore);
        free((void*)taskEndPhysCore);
        free((void*)taskNumThreads);
        free((void*)threadAssignments);
        return __LINE__;
    }
    
    for (uint32_t taskIndex = 0; taskIndex < taskCount; ++taskIndex)
        spindleInitializeLocalThreadBarrier(taskIndex, taskNumThreads[taskIndex]);
    
    // Free buffers no longer needed.
    free((void*)taskStartPhysCore);
    free((void*)taskEndPhysCore);
    free((void*)taskNumThreads);
    
    // Create the threads and wait for the result.
    threadResult = spindleCreateThreads(threadAssignments, totalNumThreads, useCurrentThread);
    
    // Free allocated memory and return.
    spindleFreeLocalThreadBarriers();
    free((void*)threadAssignments);
    return threadResult;
}
