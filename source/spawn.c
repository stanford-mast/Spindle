/*****************************************************************************
 * libspindle
 *      Multi-platform topology-aware thread control library.
 *      Distributes a set of synchronized tasks over cores in the system.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file spawn.c
 *      Implementation of all thread-spawning logic.
 *****************************************************************************/

#include "../spindle.h"

#include <hwloc.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Internal data structure, used to provide each spawned thread with control and identification information.
/// One such data structure exists per thread created during the spawning process.
/// Each instance uniquely identifies and supplies sufficient information for each thread.
typedef struct SSpindleThreadInfo
{
    TSpindleFunc func;                                                      ///< Starting function to call.
    void* arg;                                                              ///< Argument to pass to the starting function.

    hwloc_topology_t topology;                                              ///< System topology object from `hwloc`.
    hwloc_obj_t affinityObject;                                             ///< Object from `hwloc` that identifies the PU to which the present thread should be affinitized.

    uint32_t localThreadId;                                                 ///< Local thread ID. Can be obtained by calling spindleGetLocalThreadId.
    uint32_t globalThreadId;                                                ///< Global thread ID. Can be obtained by calling spindleGetGlobalThreadId.
    uint32_t threadGroupId;                                                 ///< Thread group ID. Can be obtained by calling spindleGetThreadGroupId.
    uint32_t localThreadCount;                                              ///< Number of threads in the current thread's group. Can be obtained by calling spindleGetLocalThreadCount.
    uint32_t globalThreadCount;                                             ///< Total number of threads spawned. Can be obtained by calling spindleGetGlobalThreadCount.
    uint32_t groupCount;                                                    ///< Number of thread groups created. Can be obtained by calling spindleGetGroupCount.
} SSpindleThreadInfo;


// -------- HELPERS -------------------------------------------------------- //

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

uint32_t spindleThreadsSpawn(SSpindleTaskSpec* taskSpec, uint32_t taskCount)
{
    SSpindleThreadInfo* threadAssignments = NULL;
    uint32_t nextThreadAssignmentIndex = 0;
    
    hwloc_topology_t topology;
    hwloc_obj_t numaNodeObject = NULL;
    hwloc_obj_t physicalCoreObject = NULL;

    uint32_t taskStartPhysCore[SPINDLE_MAX_TASK_COUNT];
    uint32_t taskEndPhysCore[SPINDLE_MAX_TASK_COUNT];
    uint32_t taskNumThreads[SPINDLE_MAX_TASK_COUNT];
    
    uint32_t currentNumaNode = 0;
    uint32_t threadsLeftOnCurrentNumaNode = 0;
    uint32_t coresLeftOnCurrentNumaNode = 0;
    uint32_t numNumaNodes = 0;
    uint32_t totalNumThreads = 0;
    
    // It is trivially a success case if the number of tasks is zero.
    if (0 == taskCount)
        return 0;

    // It is trivially a failure case if the number of tasks is too high.
    if (SPINDLE_MAX_TASK_COUNT < taskCount)
        return __LINE__;
    
    // Create and load the hardware topology of the current system.
    if (0 != hwloc_topology_init(&topology))
        return __LINE__;
    
    if (0 != hwloc_topology_load(topology))
        return __LINE__;

    // Figure out the highest possible NUMA node index, for error-checking purposes.
    numNumaNodes = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NUMANODE);
    if (1 > numNumaNodes)
    {
        hwloc_topology_destroy(topology);
        return __LINE__;
    }

    // Initialize data structures to assign from the first NUMA node in the system.
    numaNodeObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NUMANODE, currentNumaNode);
    if (NULL == numaNodeObject)
    {
        hwloc_topology_destroy(topology);
        return __LINE__;
    }

    threadsLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_PU);
    coresLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE);

    physicalCoreObject = hwloc_get_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, 0);
    if (NULL == physicalCoreObject)
    {
        hwloc_topology_destroy(topology);
        return __LINE__;
    }

    // Assign ranges of physical cores to tasks, based on the task specifications.
    for (uint32_t taskIndex = 0; taskIndex < taskCount; ++taskIndex)
    {
        // Verify the task specification's NUMA node.
        if (taskSpec[taskIndex].numaNode < currentNumaNode || taskSpec[taskIndex].numaNode >= numNumaNodes)
        {
            hwloc_topology_destroy(topology);
            return __LINE__;
        }

        // Reinitialize to a different NUMA node if the specified NUMA node is different.
        if (taskSpec[taskIndex].numaNode != currentNumaNode)
        {
            currentNumaNode = taskSpec[taskIndex].numaNode;
            
            numaNodeObject = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NUMANODE, currentNumaNode);
            if (NULL == numaNodeObject)
            {
                hwloc_topology_destroy(topology);
                return __LINE__;
            }

            threadsLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_PU);
            coresLeftOnCurrentNumaNode = hwloc_get_nbobjs_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE);

            physicalCoreObject = hwloc_get_obj_inside_cpuset_by_type(topology, numaNodeObject->cpuset, HWLOC_OBJ_CORE, 0);
            if (NULL == physicalCoreObject)
            {
                hwloc_topology_destroy(topology);
                return __LINE__;
            }
        }

        // Find the ending physical core for the current task, based on the number of threads specified.
        if (0 == taskSpec[taskIndex].numThreads)
        {
            // Verify that at least one core remains available on the current NUMA node.
            if (1 > coresLeftOnCurrentNumaNode)
            {
                hwloc_topology_destroy(topology);
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
            if (threadsLeftOnCurrentNumaNode < taskSpec[taskIndex].numThreads || (SpindleSMTPolicyDisableSMT == taskSpec[taskIndex].smtPolicy && coresLeftOnCurrentNumaNode < taskSpec[taskIndex].numThreads))
            {
                hwloc_topology_destroy(topology);
                return __LINE__;
            }

            // Assign the starting physical core for the current task.
            taskStartPhysCore[taskIndex] = physicalCoreObject->logical_index;

            // Specify the number of threads for the current task.
            taskNumThreads[taskIndex] = taskSpec[taskIndex].numThreads;
            
            // Assign one physical core at a time to the present task, 
            while (numThreadsAssignedForTask < taskSpec[taskIndex].numThreads)
            {
                // Calculate the number of threads consumed by the present physical core.
                const uint32_t numThreadsConsumed = hwloc_get_nbobjs_inside_cpuset_by_type(topology, physicalCoreObject->cpuset, HWLOC_OBJ_PU);

                // Check for errors: there needs to be a valid physical core object at this point.
                if (NULL == physicalCoreObject)
                {
                    hwloc_topology_destroy(topology);
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
        hwloc_topology_destroy(topology);
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
            threadAssignments[nextThreadAssignmentIndex].localThreadId = threadIndex;
            threadAssignments[nextThreadAssignmentIndex].globalThreadId = nextThreadAssignmentIndex;
            threadAssignments[nextThreadAssignmentIndex].threadGroupId = taskIndex;
            threadAssignments[nextThreadAssignmentIndex].localThreadCount = taskNumThreads[taskIndex];
            threadAssignments[nextThreadAssignmentIndex].globalThreadCount = totalNumThreads;
            threadAssignments[nextThreadAssignmentIndex].groupCount = taskCount;

            nextThreadAssignmentIndex += 1;
        }
    }
    
    // TESTING: print out thread assignment information.
    printf("Created %u groups and %u total threads...\n", taskCount, totalNumThreads);
    for (uint32_t threadIndex = 0; threadIndex < totalNumThreads; ++threadIndex)
    {
        printf(">> thread %u (%u,%u) is assigned to OS logical core %u\n", threadAssignments[threadIndex].globalThreadId, threadAssignments[threadIndex].threadGroupId, threadAssignments[threadIndex].localThreadId, threadAssignments[threadIndex].affinityObject->os_index);
    }
    
    // Destroy the hardware topology, free allocated memory, and return.
    hwloc_topology_destroy(topology);
    free((void*)threadAssignments);
    return 0;
}
