#include "allocation.h"

#include <stdexcept>

AllocationResult runFirstFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
) {


    if (blocks.empty()) {
        throw std::invalid_argument("Memory blocks cannot be empty.");
    }

    std::vector<MemoryBlock> freeBlocks = blocks;
    AllocationResult result;

    // Validate the initial memory blocks.
    for (const auto& block : blocks) {
        if (block.id <= 0 || block.size <= 0) {
            throw std::invalid_argument(
                "Block IDs and sizes must be positive."
            );
        }
    }

    // Validate process requests.
    for (const auto& process : processes) {
        if (process.pid.empty() || process.size <= 0) {
            throw std::invalid_argument(
                "Process IDs must not be empty and sizes must be positive."
            );
        }
    }

    // Process requests in their input order.
    for (const auto& process : processes) {
        AllocationDecision decision{
            process.pid,
            process.size,
            -1,
            0,
            false
        };

        // First Fit: choose the first available region that is large enough.
        for (auto& block : freeBlocks) {
            if (block.size >= process.size) {
                decision.blockId = block.id;
                decision.allocatedSize = process.size;
                decision.allocated = true;

                // Keep the unused portion available.
                block.size -= process.size;

                break;
            }
        }

        result.decisions.push_back(decision);
    }

    // Calculate total remaining free memory.
    result.totalFreeMemory = 0;

    for (const auto& block : freeBlocks) {
        result.totalFreeMemory += block.size;
    }

    return result;
}

AllocationResult runBestFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
) {
    if (blocks.empty()) {
        throw std::invalid_argument("Memory blocks cannot be empty.");
    }

    std::vector<MemoryBlock> freeBlocks = blocks;
    AllocationResult result;
    result.totalFreeMemory = 0;

    // Validate memory blocks.
    for (const auto& block : blocks) {
        if (block.id <= 0 || block.size <= 0) {
            throw std::invalid_argument(
                "Block IDs and sizes must be positive."
            );
        }
    }

    // Validate process requests.
    for (const auto& process : processes) {
        if (process.pid.empty() || process.size <= 0) {
            throw std::invalid_argument(
                "Process IDs must not be empty and sizes must be positive."
            );
        }
    }

    // Process each request in input order.
    for (const auto& process : processes) {
        AllocationDecision decision{
            process.pid,
            process.size,
            -1,
            0,
            false
        };

        int bestIndex = -1;

        // Find the smallest free region that can accommodate the process.
        for (int i = 0; i < static_cast<int>(freeBlocks.size()); i++) {
            if (freeBlocks[i].size >= process.size) {
                if (bestIndex == -1 ||
                    freeBlocks[i].size < freeBlocks[bestIndex].size) {
                    bestIndex = i;
                }
            }
        }

        // Allocate memory if a suitable region exists.
        if (bestIndex != -1) {
            decision.blockId = freeBlocks[bestIndex].id;
            decision.allocatedSize = process.size;
            decision.allocated = true;

            freeBlocks[bestIndex].size -= process.size;
        }

        result.decisions.push_back(decision);
    }

    // Calculate remaining free memory.
    for (const auto& block : freeBlocks) {
        result.totalFreeMemory += block.size;
    }

    return result;
}


AllocationResult runWorstFit(
    const std::vector<MemoryBlock>& blocks,
    const std::vector<MemoryRequest>& processes
) {
    if (blocks.empty()) {
        throw std::invalid_argument("Memory blocks cannot be empty.");
    }

    std::vector<MemoryBlock> freeBlocks = blocks;
    AllocationResult result;
    result.totalFreeMemory = 0;

    // Validate memory blocks.
    for (const auto& block : blocks) {
        if (block.id <= 0 || block.size <= 0) {
            throw std::invalid_argument(
                "Block IDs and sizes must be positive."
            );
        }
    }

    // Validate process requests.
    for (const auto& process : processes) {
        if (process.pid.empty() || process.size <= 0) {
            throw std::invalid_argument(
                "Process IDs must not be empty and sizes must be positive."
            );
        }
    }

    // Process each request in input order.
    for (const auto& process : processes) {
        AllocationDecision decision{
            process.pid,
            process.size,
            -1,
            0,
            false
        };

        int worstIndex = -1;

        // Find the largest free region that can accommodate the process.
        for (int i = 0; i < static_cast<int>(freeBlocks.size()); i++) {
            if (freeBlocks[i].size >= process.size) {
                if (worstIndex == -1 ||
                    freeBlocks[i].size > freeBlocks[worstIndex].size) {
                    worstIndex = i;
                }
            }
        }

        // Allocate memory if a suitable region exists.
        if (worstIndex != -1) {
            decision.blockId = freeBlocks[worstIndex].id;
            decision.allocatedSize = process.size;
            decision.allocated = true;

            freeBlocks[worstIndex].size -= process.size;
        }

        result.decisions.push_back(decision);
    }

    // Calculate remaining free memory.
    for (const auto& block : freeBlocks) {
        result.totalFreeMemory += block.size;
    }

    return result;
}

