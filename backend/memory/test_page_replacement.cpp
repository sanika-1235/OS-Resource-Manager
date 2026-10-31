#include "page_replacement.h"

#include <iostream>
#include <string>

void displayResult(
    const std::string& algorithmName,
    const PageReplacementResult& result
) {
    std::cout << "\n========== " << algorithmName
              << " ==========\n\n";

    std::cout << "Page\tFrames\t\tResult\n";

    for (const auto& step : result.steps) {
        std::cout << step.page << "\t";

        for (int frame : step.frames) {
            if (frame == -1) {
                std::cout << "- ";
            } else {
                std::cout << frame << " ";
            }
        }

        std::cout << "\t\t"
                  << (step.pageFault ? "Fault" : "Hit")
                  << '\n';
    }

    std::cout << "\nTotal Hits: " << result.hits;
    std::cout << "\nTotal Faults: " << result.faults;
    std::cout << "\nHit Ratio: " << result.hitRatio;
    std::cout << "\nFault Ratio: " << result.faultRatio;

    std::cout << "\n";
}

int main() {
   PageReplacementInput input{
    {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5},
    3
};

    displayResult("FIFO", runFIFO(input));
    displayResult("LRU", runLRU(input));
    displayResult("OPTIMAL", runOptimal(input));

    return 0;
}