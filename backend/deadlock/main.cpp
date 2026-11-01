
#include "DeadlockManager.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    int processes, resources;

    cin >> processes >> resources;

    if (processes <= 0 || processes > 10 ||
        resources <= 0 || resources > 5) {
        cout << "Invalid process or resource count";
        return 0;
    }

    vector<int> available(resources);

    for (int j = 0; j < resources; j++)
        cin >> available[j];

    vector<vector<int>> allocation(
        processes, vector<int>(resources)
    );

    vector<vector<int>> maximum(
        processes, vector<int>(resources)
    );

    for (int i = 0; i < processes; i++)
        for (int j = 0; j < resources; j++)
            cin >> allocation[i][j];

    for (int i = 0; i < processes; i++)
        for (int j = 0; j < resources; j++)
            cin >> maximum[i][j];

    DeadlockManager manager(
        processes, resources,
        allocation, maximum, available
    );

    manager.displayResult();

    return 0;
}
