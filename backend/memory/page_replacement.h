#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include <vector>

struct PageReplacementInput {
    std::vector<int> referenceString;
    int frameCount;
};

struct PageStep {
    int page;
    std::vector<int> frames;
    bool pageFault;
};

struct PageReplacementResult {
    std::vector<PageStep> steps;
    int hits;
    int faults;
    double hitRatio;
    double faultRatio;
};

PageReplacementResult runFIFO(
    const PageReplacementInput& input
);

PageReplacementResult runLRU(
    const PageReplacementInput& input
);

PageReplacementResult runOptimal(
    const PageReplacementInput& input
);

#endif