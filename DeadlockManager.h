
#ifndef DEADLOCK_MANAGER_H
#define DEADLOCK_MANAGER_H

#include <vector>
#include <string>

using namespace std;

class DeadlockManager {
private:
    int processCount;
    int resourceCount;

    vector<vector<int>> allocation;
    vector<vector<int>> maximum;
    vector<vector<int>> need;
    vector<int> available;

    void calculateNeed();

public:
    DeadlockManager(
        int processes,
        int resources,
        vector<vector<int>> alloc,
        vector<vector<int>> max,
        vector<int> avail
    );

    bool validateInput();
    bool isSafe(vector<int>& safeSequence);

    void displayNeed();
    void displayResult();

    static void explainDeadlockConditions();
};

#endif
