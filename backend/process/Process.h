
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
using namespace std;

// Process states
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

// Process Control Block (PCB)
struct Process {

    // Basic process information
    int pid;
    string name;

    // CPU-related information
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;

    // Current process state
    ProcessState state;

    // Constructor
    Process(int id, string processName,
            int arrival, int burst, int priorityValue)
        : pid(id),
          name(processName),
          arrivalTime(arrival),
          burstTime(burst),
          remainingTime(burst),
          priority(priorityValue),
          state(ProcessState::NEW) {
    }
};

// Convert process state into readable text
string getStateName(ProcessState state);

#endif
