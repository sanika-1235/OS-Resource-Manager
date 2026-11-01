
#include "DeadlockManager.h"
#include <iostream>
#include <algorithm>

using namespace std;

DeadlockManager::DeadlockManager(
    int processes,
    int resources,
    vector<vector<int>> alloc,
    vector<vector<int>> max,
    vector<int> avail
) {
    processCount = processes;
    resourceCount = resources;
    allocation = alloc;
    maximum = max;
    available = avail;

    need.resize(processCount, vector<int>(resourceCount, 0));

    calculateNeed();
}

void DeadlockManager::calculateNeed() {
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < resourceCount; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

bool DeadlockManager::validateInput() {
    if (processCount <= 0 || resourceCount <= 0)
        return false;

    if (allocation.size() != processCount ||
        maximum.size() != processCount ||
        available.size() != resourceCount)
        return false;

    for (int i = 0; i < processCount; i++) {
        if (allocation[i].size() != resourceCount ||
            maximum[i].size() != resourceCount)
            return false;

        for (int j = 0; j < resourceCount; j++) {
            if (allocation[i][j] < 0 ||
                maximum[i][j] < 0 ||
                allocation[i][j] > maximum[i][j])
                return false;
        }
    }

    for (int value : available) {
        if (value < 0)
            return false;
    }

    return true;
}

bool DeadlockManager::isSafe(vector<int>& safeSequence) {
    safeSequence.clear();

    if (!validateInput())
        return false;

    vector<int> work = available;
    vector<bool> finish(processCount, false);

    int completed = 0;

    while (completed < processCount) {
        bool found = false;

        for (int i = 0; i < processCount; i++) {
            if (finish[i])
                continue;

            bool canExecute = true;

            for (int j = 0; j < resourceCount; j++) {
                if (need[i][j] > work[j]) {
                    canExecute = false;
                    break;
                }
            }

            if (canExecute) {
                for (int j = 0; j < resourceCount; j++) {
                    work[j] += allocation[i][j];
                }

                finish[i] = true;
                safeSequence.push_back(i);
                completed++;
                found = true;
            }
        }

        if (!found)
            break;
    }

    return completed == processCount;
}

void DeadlockManager::displayNeed() {
    cout << "\nNeed Matrix:\n";

    for (int i = 0; i < processCount; i++) {
        cout << "P" << i << ": ";

        for (int j = 0; j < resourceCount; j++) {
            cout << need[i][j] << " ";
        }

        cout << endl;
    }
}

void DeadlockManager::displayResult() {
    vector<int> sequence;

    displayNeed();

    if (isSafe(sequence)) {
        cout << "\nSystem is in SAFE state.\n";
        cout << "Safe Sequence: ";

        for (int i = 0; i < sequence.size(); i++) {
            cout << "P" << sequence[i];

            if (i != sequence.size() - 1)
                cout << " -> ";
        }

        cout << endl;
    } else {
        cout << "\nSystem is in UNSAFE state.\n";
        cout << "No complete safe sequence exists.\n";
    }
}

void DeadlockManager::explainDeadlockConditions() {
    cout << "\nFour Necessary Conditions for Deadlock:\n";

    cout << "1. Mutual Exclusion: A resource can be used by "
         << "only one process at a time.\n";

    cout << "2. Hold and Wait: A process holds resources "
         << "while waiting for additional resources.\n";

    cout << "3. No Preemption: Resources cannot be forcibly "
         << "taken from a process.\n";

    cout << "4. Circular Wait: Processes form a circular "
         << "chain of resource waiting.\n";
}
