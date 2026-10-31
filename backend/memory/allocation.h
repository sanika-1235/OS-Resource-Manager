#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <string>
#include <vector>

struct MemoryBlock {
    int id;
    int size;
};

struct MemoryRequest {
    std::string pid;
    int size;
};

struct AllocationDecision {
    std::string pid;
    int requestedSize;
    int blockId;
    int allocatedSize;
    bool allocated;
};

struct AllocationResult {
    std::vector<AllocationDecision> decisions;
    int totalFreeMemory;
};

AllocationResult runFirstFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
);


AllocationResult runBestFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
);

AllocationResult runWorstFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
);

#endif