
#include "ProcessManager.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Search process using PID
Process* ProcessManager::findProcess(int pid) {
    for (auto &p : processes) {
        if (p.pid == pid) {
            return &p;
        }
    }

    return nullptr;
}

// Create process
bool ProcessManager::createProcess(
    int pid, string name,
    int arrival, int burst, int priority) {

    if (pid <= 0 || name.empty() ||
        arrival < 0 || burst <= 0 || priority < 0) {
        cout << "Invalid process details!\n";
        return false;
    }

    if (findProcess(pid) != nullptr) {
        cout << "Duplicate PID!\n";
        return false;
    }

    processes.emplace_back(
        pid, name, arrival, burst, priority
    );

    cout << "Process created successfully!\n";
    return true;
}

// Update process state
bool ProcessManager::updateState(
    int pid, ProcessState newState) {

    Process* p = findProcess(pid);

    if (p == nullptr) {
        cout << "Process not found!\n";
        return false;
    }

    if (p->state == ProcessState::TERMINATED) {
        cout << "Process already terminated!\n";
        return false;
    }

    bool valid = false;

    switch (p->state) {
        case ProcessState::NEW:
            valid = (newState == ProcessState::READY ||
                     newState == ProcessState::TERMINATED);
            break;

        case ProcessState::READY:
            valid = (newState == ProcessState::RUNNING ||
                     newState == ProcessState::TERMINATED);
            break;

        case ProcessState::RUNNING:
            valid = (newState == ProcessState::READY ||
                     newState == ProcessState::WAITING ||
                     newState == ProcessState::TERMINATED);
            break;

        case ProcessState::WAITING:
            valid = (newState == ProcessState::READY ||
                     newState == ProcessState::TERMINATED);
            break;

        case ProcessState::TERMINATED:
            valid = false;
            break;
    }

    if (!valid) {
        cout << "Invalid state transition!\n";
        return false;
    }

    p->state = newState;

    cout << "Process state updated!\n";
    return true;
}

// Terminate process
bool ProcessManager::terminateProcess(int pid) {
    return updateState(pid, ProcessState::TERMINATED);
}

// Display process table
void ProcessManager::displayProcesses() {
    cout << "\n========== PROCESS TABLE ==========\n";

    cout << left
         << setw(10) << "PID"
         << setw(15) << "Name"
         << setw(12) << "Arrival"
         << setw(12) << "Burst"
         << setw(12) << "Remaining"
         << setw(12) << "Priority"
         << setw(15) << "State"
         << endl;

    for (const auto &p : processes) {
        cout << left
             << setw(10) << p.pid
             << setw(15) << p.name
             << setw(12) << p.arrivalTime
             << setw(12) << p.burstTime
             << setw(12) << p.remainingTime
             << setw(12) << p.priority
             << setw(15) << getStateName(p.state)
             << endl;
    }
}

// Return process list
vector<Process> ProcessManager::getAllProcesses() const {
    return processes;
}
