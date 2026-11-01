#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <string>
#include <vector>

// Represents a request from a process for memory.
struct MemoryRequest {
    std::string pid;
    int size;
};

// Represents a physical memory region.
struct MemoryRegion {
    int id;
    int startAddress;
    int size;
    bool allocated;
    std::string pid;
};

// Represents the result of allocating memory to one process.
struct AllocationDecision {
    std::string pid;
    int requestedSize;
    int blockId;
    int startAddress;
    int allocatedSize;
    bool allocated;
};

// Stores memory allocation statistics and the final memory layout.
struct AllocationResult {
    std::vector<AllocationDecision> decisions;
    std::vector<MemoryRegion> memoryLayout;
    int totalFreeMemory;
    int largestFreeBlock;
    int freeBlockCount;
    double externalFragmentation;
};

// First Fit, Best Fit, and Worst Fit allocation algorithms.
AllocationResult runFirstFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
);

AllocationResult runBestFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
);

AllocationResult runWorstFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
);

// Releases memory allocated to a process.
AllocationResult deallocateProcess(
    const AllocationResult& currentState,
    const std::string& pid
);

#endif