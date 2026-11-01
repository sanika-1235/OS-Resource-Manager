#include "allocation.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_set>

// Validate memory regions and process requests.
static void validateInput(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
) {
    if (regions.empty()) {
        throw std::invalid_argument("Memory regions cannot be empty.");
    }

    std::unordered_set<int> regionIds;
    std::unordered_set<std::string> processIds;

    for (const auto& region : regions) {
        if (region.id <= 0 || region.startAddress < 0 || region.size <= 0) {
            throw std::invalid_argument(
                "Region IDs and sizes must be positive, and addresses non-negative."
            );
        }

        if (!regionIds.insert(region.id).second) {
            throw std::invalid_argument("Memory region IDs must be unique.");
        }
    }

    for (const auto& process : processes) {
        if (process.pid.empty() || process.size <= 0) {
            throw std::invalid_argument(
                "Process IDs must not be empty and sizes must be positive."
            );
        }

        if (!processIds.insert(process.pid).second) {
            throw std::invalid_argument("Process IDs must be unique.");
        }
    }
}

// Calculate memory statistics for the current layout.
static void calculateStatistics(AllocationResult& result) {
    result.totalFreeMemory = 0;
    result.largestFreeBlock = 0;
    result.freeBlockCount = 0;

    for (const auto& region : result.memoryLayout) {
        if (!region.allocated) {
            result.totalFreeMemory += region.size;
            result.largestFreeBlock =
                std::max(result.largestFreeBlock, region.size);
            result.freeBlockCount++;
        }
    }

    if (result.totalFreeMemory > 0) {
        result.externalFragmentation =
            100.0 * (result.totalFreeMemory - result.largestFreeBlock)
            / result.totalFreeMemory;
    } else {
        result.externalFragmentation = 0.0;
    }
}

// Merge adjacent free regions.
static void mergeFreeRegions(AllocationResult& result) {
    std::sort(
        result.memoryLayout.begin(),
        result.memoryLayout.end(),
        [](const MemoryRegion& a, const MemoryRegion& b) {
            return a.startAddress < b.startAddress;
        }
    );

    std::vector<MemoryRegion> merged;

    for (const auto& region : result.memoryLayout) {
        if (!merged.empty()) {
            MemoryRegion& previous = merged.back();

            if (!previous.allocated &&
                !region.allocated &&
                previous.startAddress + previous.size == region.startAddress) {

                previous.size += region.size;
                continue;
            }
        }

        merged.push_back(region);
    }

    result.memoryLayout = merged;
}

// Allocate memory using the selected strategy.
static AllocationResult allocateMemory(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes,
    int strategy
) {
    validateInput(regions, processes);

    AllocationResult result{};
    result.memoryLayout = regions;

    // Sort regions by physical address.
    std::sort(
        result.memoryLayout.begin(),
        result.memoryLayout.end(),
        [](const MemoryRegion& a, const MemoryRegion& b) {
            return a.startAddress < b.startAddress;
        }
    );

    // Generate unique IDs for newly created memory regions.
int nextRegionId = 1;

for (const auto& region : result.memoryLayout) {
    if (region.id >= nextRegionId) {
        nextRegionId = region.id + 1;
    }
}

    // Validate that regions do not overlap.
    for (std::size_t i = 1; i < result.memoryLayout.size(); i++) {
        const MemoryRegion& previous = result.memoryLayout[i - 1];
        const MemoryRegion& current = result.memoryLayout[i];

        if (previous.startAddress + previous.size > current.startAddress) {
            throw std::invalid_argument(
                "Memory regions must not overlap."
            );
        }
    }

    // Process each request in input order.
    for (const auto& process : processes) {
        AllocationDecision decision{
            process.pid,
            process.size,
            -1,
            -1,
            0,
            false
        };

        int selectedIndex = -1;

        for (int i = 0;
             i < static_cast<int>(result.memoryLayout.size());
             i++) {

            const MemoryRegion& region = result.memoryLayout[i];

            if (region.allocated || region.size < process.size) {
                continue;
            }

            if (selectedIndex == -1) {
                selectedIndex = i;
                continue;
            }

            const int selectedSize =
                result.memoryLayout[selectedIndex].size;

            if (strategy == 1 && region.size < selectedSize) {
                // Best Fit: choose the smallest suitable region.
                selectedIndex = i;
            } else if (strategy == 2 && region.size > selectedSize) {
                // Worst Fit: choose the largest suitable region.
                selectedIndex = i;
            }

            // First Fit keeps the first suitable region.
        }

        if (selectedIndex != -1) {
            MemoryRegion original = result.memoryLayout[selectedIndex];

            decision.blockId = original.id;
            decision.startAddress = original.startAddress;
            decision.allocatedSize = process.size;
            decision.allocated = true;

            // Replace the free region with the allocated region.
            MemoryRegion allocatedRegion{
                original.id,
                original.startAddress,
                process.size,
                true,
                process.pid
            };

            result.memoryLayout[selectedIndex] = allocatedRegion;

            // Create a remaining free region when space is left over.
            int remainingSize = original.size - process.size;

            if (remainingSize > 0) {
               MemoryRegion remainingRegion{
    nextRegionId++,
    original.startAddress + process.size,
    remainingSize,
    false,
    ""
};

                result.memoryLayout.insert(
                    result.memoryLayout.begin() + selectedIndex + 1,
                    remainingRegion
                );
            }
        }

        result.decisions.push_back(decision);
    }

    calculateStatistics(result);
    return result;
}

AllocationResult runFirstFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
) {
    return allocateMemory(regions, processes, 0);
}

AllocationResult runBestFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
) {
    return allocateMemory(regions, processes, 1);
}

AllocationResult runWorstFit(
    const std::vector<MemoryRegion>& regions,
    const std::vector<MemoryRequest>& processes
) {
    return allocateMemory(regions, processes, 2);
}

// Release a process's memory and merge adjacent free regions.
AllocationResult deallocateProcess(
    const AllocationResult& currentState,
    const std::string& pid
) {
    if (pid.empty()) {
        throw std::invalid_argument("Process ID cannot be empty.");
    }

    AllocationResult result = currentState;
    bool found = false;

    for (auto& region : result.memoryLayout) {
        if (region.allocated && region.pid == pid) {
            region.allocated = false;
            region.pid.clear();
            found = true;
        }
    }

    if (!found) {
        throw std::invalid_argument(
            "The specified process has no allocated memory."
        );
    }

    mergeFreeRegions(result);
    calculateStatistics(result);

    return result;
}