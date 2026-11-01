#include "allocation.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

void displayResult(
    const std::string& algorithmName,
    const AllocationResult& result
) {
    std::cout << "\n========== " << algorithmName
              << " ==========\n";

    std::cout << "\nAllocation Decisions:\n";
    std::cout << std::left
              << std::setw(10) << "Process"
              << std::setw(12) << "Requested"
              << std::setw(12) << "Status"
              << std::setw(12) << "Region ID"
              << std::setw(15) << "Start Address"
              << '\n';

    for (const auto& decision : result.decisions) {
        std::cout << std::left
                  << std::setw(10) << decision.pid
                  << std::setw(12) << decision.requestedSize
                  << std::setw(12)
                  << (decision.allocated ? "Allocated" : "Failed")
                  << std::setw(12);

        if (decision.allocated) {
            std::cout << decision.blockId
                      << std::setw(15) << decision.startAddress;
        } else {
            std::cout << "-"
                      << std::setw(15) << "-";
        }

        std::cout << '\n';
    }

    std::cout << "\nFinal Memory Layout:\n";
    std::cout << std::left
              << std::setw(12) << "Region ID"
              << std::setw(15) << "Start Address"
              << std::setw(12) << "Size"
              << std::setw(12) << "Status"
              << "Process\n";

    for (const auto& region : result.memoryLayout) {
        std::cout << std::left
                  << std::setw(12) << region.id
                  << std::setw(15) << region.startAddress
                  << std::setw(12) << region.size
                  << std::setw(12)
                  << (region.allocated ? "Allocated" : "Free")
                  << (region.allocated ? region.pid : "-")
                  << '\n';
    }

    std::cout << "\nMemory Statistics:\n";
    std::cout << "Total free memory: "
              << result.totalFreeMemory << " KB\n";

    std::cout << "Largest free region: "
              << result.largestFreeBlock << " KB\n";

    std::cout << "Number of free regions: "
              << result.freeBlockCount << '\n';

    std::cout << "External fragmentation indicator: "
              << std::fixed << std::setprecision(2)
              << result.externalFragmentation << "%\n";
}

int main() {
    const std::vector<MemoryRegion> regions = {
        {1, 0,   100, false, ""},
        {2, 100, 300, false, ""},
        {3, 400, 200, false, ""},
        {4, 600, 400, false, ""}
    };

    const std::vector<MemoryRequest> processes = {
        {"P1", 180},
        {"P2", 90},
        {"P3", 250}
    };

    try {
        AllocationResult firstFit = runFirstFit(regions, processes);
        displayResult("FIRST FIT", firstFit);

        AllocationResult bestFit = runBestFit(regions, processes);
        displayResult("BEST FIT", bestFit);

        AllocationResult worstFit = runWorstFit(regions, processes);
        displayResult("WORST FIT", worstFit);

        // Demonstrate deallocation using the First Fit result.
        std::cout << "\n========== DEALLOCATION TEST ==========\n";

        AllocationResult afterDeallocation =
    deallocateProcess(firstFit, "P1");

displayResult("AFTER RELEASING P1", afterDeallocation);

// Verify that P1's memory was released and adjacent free
// regions were merged correctly.
bool mergedRegionFound = false;

for (const auto& region : afterDeallocation.memoryLayout) {
    if (!region.allocated &&
        region.startAddress == 90 &&
        region.size == 510) {
        mergedRegionFound = true;
        break;
    }
}

if (!mergedRegionFound) {
    std::cerr << "TEST FAILED: Adjacent free regions were not merged correctly.\n";
    return 1;
}

if (afterDeallocation.totalFreeMemory != 660) {
    std::cerr << "TEST FAILED: Total free memory is incorrect.\n";
    return 1;
}

std::cout << "\nTEST PASSED: Memory deallocation and merging work correctly.\n";

     
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }


        // Test that overlapping memory regions are rejected.
    std::cout << "\n========== INVALID MEMORY TEST ==========\n";

    const std::vector<MemoryRegion> invalidRegions = {
        {1, 0, 100, false, ""},
        {2, 50, 100, false, ""}
    };

    try {
        runFirstFit(invalidRegions, processes);

        std::cerr
            << "TEST FAILED: Overlapping memory regions were accepted.\n";

        return 1;
    } catch (const std::invalid_argument& e) {
        std::cout
            << "TEST PASSED: Overlapping memory regions were rejected.\n";
        std::cout << "Reason: " << e.what() << '\n';
    }


        // Test that zero-sized process requests are rejected.
    std::cout << "\n========== INVALID PROCESS TEST ==========\n";

    const std::vector<MemoryRequest> invalidProcesses = {
        {"P4", 0}
    };

    bool invalidRequestRejected = false;

    try {
        runFirstFit(regions, invalidProcesses);
    } catch (const std::invalid_argument& e) {
        invalidRequestRejected = true;
        std::cout << "Reason: " << e.what() << '\n';
    }

    if (!invalidRequestRejected) {
        std::cerr << "TEST FAILED: Zero-sized process was accepted.\n";
        return 1;
    }

    std::cout << "TEST PASSED: Zero-sized process was rejected.\n";

    return 0;
}