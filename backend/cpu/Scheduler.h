
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>

using namespace std;

// CPU Scheduling input details
struct CPUProcess {
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

// One section of the Gantt chart
// pid = -1 means CPU is idle
struct GanttSegment {
    int pid;
    int startTime;
    int endTime;
};

// Scheduling decision information
struct DecisionStep {
    int time;
    int selectedPid;
    string reason;
};

// Complete scheduling output
struct SchedulingResult {
    vector<ProcessMetrics> metrics;
    vector<GanttSegment> ganttChart;
    vector<DecisionStep> decisions;

    double averageTurnaroundTime;
    double averageWaitingTime;
    double averageResponseTime;
};

// CPU Scheduling algorithms
SchedulingResult runFCFS(const vector<CPUProcess>& processes);

SchedulingResult runSJF(const vector<CPUProcess>& processes);

SchedulingResult runSRTF(const vector<CPUProcess>& processes);

SchedulingResult runPriority(const vector<CPUProcess>& processes);

SchedulingResult runRoundRobin(
    const vector<CPUProcess>& processes,
    int timeQuantum
);

#endif
