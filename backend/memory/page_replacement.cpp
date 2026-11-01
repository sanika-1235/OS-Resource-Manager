#include "page_replacement.h"

#include <algorithm>
#include <stdexcept>
#include <queue>

PageReplacementResult runFIFO(
    const PageReplacementInput& input
) {
    if (input.referenceString.empty()) {
        throw std::invalid_argument(
            "Page reference string cannot be empty."
        );
    }

    if (input.frameCount <= 0) {
        throw std::invalid_argument(
            "Frame count must be greater than zero."
        );
    }

    PageReplacementResult result;
    result.hits = 0;
    result.faults = 0;
    result.hitRatio = 0.0;
    result.faultRatio = 0.0;

    // -1 represents an empty frame.
    std::vector<int> frames(input.frameCount, -1);

    // Tracks the order in which pages entered memory.
    std::queue<int> fifoQueue;

    for (int page : input.referenceString) {
        bool pageFound = false;

        // Check whether the requested page is already in memory.
        for (int frame : frames) {
            if (frame == page) {
                pageFound = true;
                break;
            }
        }

        if (pageFound) {
            // Page hit: FIFO order remains unchanged.
            result.hits++;
        } else {
            // Page fault.
            result.faults++;

            auto emptyFrame = std::find(
                frames.begin(),
                frames.end(),
                -1
            );

            if (emptyFrame != frames.end()) {
                // Fill an empty frame.
                *emptyFrame = page;
            } else {
                // Replace the page that entered memory first.
                int oldestPage = fifoQueue.front();
                fifoQueue.pop();

                auto oldPagePosition = std::find(
                    frames.begin(),
                    frames.end(),
                    oldestPage
                );

                *oldPagePosition = page;
            }

            // Add the newly loaded page to the FIFO order.
            fifoQueue.push(page);
        }

        // Record the frame contents after this reference.
        result.steps.push_back({
            page,
            frames,
            !pageFound
        });
    }

    int totalReferences =
        static_cast<int>(input.referenceString.size());

    result.hitRatio =
        static_cast<double>(result.hits) / totalReferences;

    result.faultRatio =
        static_cast<double>(result.faults) / totalReferences;

    return result;
}


PageReplacementResult runLRU(
    const PageReplacementInput& input
) {
    if (input.referenceString.empty()) {
        throw std::invalid_argument(
            "Page reference string cannot be empty."
        );
    }

    if (input.frameCount <= 0) {
        throw std::invalid_argument(
            "Frame count must be greater than zero."
        );
    }

    PageReplacementResult result;
    result.hits = 0;
    result.faults = 0;
    result.hitRatio = 0.0;
    result.faultRatio = 0.0;

    // -1 represents an empty frame.
    std::vector<int> frames(input.frameCount, -1);

    // Stores the most recent access time for each page.
    std::vector<int> lastUsed(
        input.frameCount,
        -1
    );

    for (int time = 0;
         time < static_cast<int>(input.referenceString.size());
         time++) {

        int page = input.referenceString[time];
        int pageIndex = -1;

        // Check whether the page is already in memory.
        for (int i = 0; i < input.frameCount; i++) {
            if (frames[i] == page) {
                pageIndex = i;
                break;
            }
        }

        bool pageFault = (pageIndex == -1);

        if (!pageFault) {
            // Page hit: update its recent access time.
            result.hits++;
            lastUsed[pageIndex] = time;
        } else {
            result.faults++;

            // First, look for an empty frame.
            int emptyIndex = -1;

            for (int i = 0; i < input.frameCount; i++) {
                if (frames[i] == -1) {
                    emptyIndex = i;
                    break;
                }
            }

            if (emptyIndex != -1) {
                // Load the page into an empty frame.
                frames[emptyIndex] = page;
                lastUsed[emptyIndex] = time;
            } else {
                // Find the least recently used page.
                int lruIndex = 0;

                for (int i = 1; i < input.frameCount; i++) {
                    if (lastUsed[i] < lastUsed[lruIndex]) {
                        lruIndex = i;
                    }
                }

                // Replace the least recently used page.
                frames[lruIndex] = page;
                lastUsed[lruIndex] = time;
            }
        }

        // Record frame contents after each reference.
        result.steps.push_back({
            page,
            frames,
            pageFault
        });
    }

    int totalReferences =
        static_cast<int>(input.referenceString.size());

    result.hitRatio =
        static_cast<double>(result.hits) / totalReferences;

    result.faultRatio =
        static_cast<double>(result.faults) / totalReferences;

    return result;
}


PageReplacementResult runOptimal(
    const PageReplacementInput& input
) {
    if (input.referenceString.empty()) {
        throw std::invalid_argument(
            "Page reference string cannot be empty."
        );
    }

    if (input.frameCount <= 0) {
        throw std::invalid_argument(
            "Frame count must be greater than zero."
        );
    }

    PageReplacementResult result;
    result.hits = 0;
    result.faults = 0;
    result.hitRatio = 0.0;
    result.faultRatio = 0.0;

    // -1 represents an empty frame.
    std::vector<int> frames(input.frameCount, -1);

    // Process each page in the reference string.
    for (int i = 0;
         i < static_cast<int>(input.referenceString.size());
         i++) {

        int page = input.referenceString[i];
        bool pageFound = false;

        // Check whether the page is already in memory.
        for (int j = 0; j < input.frameCount; j++) {
            if (frames[j] == page) {
                pageFound = true;
                break;
            }
        }

        bool pageFault = !pageFound;

        if (!pageFault) {
            // Page hit: no replacement is needed.
            result.hits++;
        } else {
            result.faults++;

            // Find an empty frame, if one exists.
            int emptyIndex = -1;

            for (int j = 0; j < input.frameCount; j++) {
                if (frames[j] == -1) {
                    emptyIndex = j;
                    break;
                }
            }

            if (emptyIndex != -1) {
                // Load the page into the empty frame.
                frames[emptyIndex] = page;
            } else {
                // Find the page whose next use is farthest
                // in the future.
                int replaceIndex = -1;
                int farthestUse = -1;

                for (int j = 0; j < input.frameCount; j++) {
                    int nextUse = -1;

                    for (int k = i + 1;
                         k < static_cast<int>(
                             input.referenceString.size());
                         k++) {

                        if (input.referenceString[k] == frames[j]) {
                            nextUse = k;
                            break;
                        }
                    }

                    // If this page is never used again,
                    // replace it immediately.
                    if (nextUse == -1) {
                        replaceIndex = j;
                        break;
                    }

                    // Otherwise, track the farthest next use.
                    if (nextUse > farthestUse) {
                        farthestUse = nextUse;
                        replaceIndex = j;
                    }
                }

                // Replace the selected page.
                frames[replaceIndex] = page;
            }
        }

        // Save the frame contents after this page reference.
        result.steps.push_back({
            page,
            frames,
            pageFault
        });
    }

    int totalReferences =
        static_cast<int>(input.referenceString.size());

    result.hitRatio =
        static_cast<double>(result.hits) / totalReferences;

    result.faultRatio =
        static_cast<double>(result.faults) / totalReferences;

    return result;
}
