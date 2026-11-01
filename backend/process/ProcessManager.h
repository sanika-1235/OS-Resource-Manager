
#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "Process.h"
#include <vector>

class ProcessManager {

private:
    // Stores all processes
    vector<Process> processes;

public:

    // Create a new process
    bool createProcess(int pid, string name,
                       int arrival, int burst,
                       int priority);

    // Find process using PID
    Process* findProcess(int pid);

    // Update process state
    bool updateState(int pid, ProcessState newState);

    // Terminate process
    bool terminateProcess(int pid);

    // Display all processes
    void displayProcesses();

    // Return all processes
    vector<Process> getAllProcesses() const;
};

#endif
