#include "allocation.h"

#include <iostream>
#include <string>
#include <vector>

void displayResult(
    const std::string& algorithmName,
    const AllocationResult& result
) {
    std::cout << "\n========== " << algorithmName
              << " ==========\n";

    for (const auto& decision : result.decisions) {
        std::cout << "Process: " << decision.pid
                  << " | Requested: " << decision.requestedSize << " KB";

        if (decision.allocated) {
            std::cout << " | Allocated to Block: B"
                      << decision.blockId;
        } else {
            std::cout << " | Allocation Failed";
        }

        std::cout << '\n';
    }

    std::cout << "Total Free Memory: "
              << result.totalFreeMemory << " KB\n";
}


int main() {
    std::vector<MemoryBlock> blocks = {
        {1, 100},
        {2, 300},
        {3, 200},
        {4, 400}
    };

    std::vector<MemoryRequest> processes = {
        {"P1", 180},
        {"P2", 90},
        {"P3", 250}
    };

    displayResult("FIRST FIT", runFirstFit(blocks, processes));
    displayResult("BEST FIT", runBestFit(blocks, processes));
    displayResult("WORST FIT", runWorstFit(blocks, processes));

    return 0;
}
