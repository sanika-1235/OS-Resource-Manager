#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>

using namespace std;

// Input details for each process
struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
};

// Performance metrics for each process
struct ProcessMetrics {
    int pid;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

// Represents one section of the Gantt chart
// pid = -1 means CPU is idle
struct GanttSegment {
    int pid;
    int startTime;
    int endTime;
};

// Stores why the scheduler selected a process
struct DecisionStep {
    int time;
    int selectedPid;
    string reason;
};

// Complete output of a scheduling algorithm
struct SchedulingResult {
    vector<ProcessMetrics> metrics;
    vector<GanttSegment> ganttChart;
    vector<DecisionStep> decisions;

    double averageTurnaroundTime;
    double averageWaitingTime;
    double averageResponseTime;
};

// CPU Scheduling algorithms
SchedulingResult runFCFS(const vector<Process>& processes);

SchedulingResult runSJF(const vector<Process>& processes);

SchedulingResult runSRTF(const vector<Process>& processes);

SchedulingResult runPriority(const vector<Process>& processes);

SchedulingResult runRoundRobin(
    const vector<Process>& processes,
    int timeQuantum
);

#endif