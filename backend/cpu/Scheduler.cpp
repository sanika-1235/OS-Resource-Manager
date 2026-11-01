
#include "Scheduler.h"
#include <algorithm>
#include <queue>


// First Come, First Served (FCFS)
SchedulingResult runFCFS(const vector<Process>& processes)
{
    SchedulingResult result;

    result.averageTurnaroundTime = 0;
    result.averageWaitingTime = 0;
    result.averageResponseTime = 0;

    int n = processes.size();

    if (n == 0)
        return result;

    // Keep the original index so metrics remain in input order
    vector<pair<Process, int>> order;

    for (int i = 0; i < n; i++)
        order.push_back({processes[i], i});

    // Sort according to arrival time
    // stable_sort preserves input order when arrival times are equal
    stable_sort(order.begin(), order.end(),
        [](const pair<Process, int>& a,
           const pair<Process, int>& b)
        {
            return a.first.arrivalTime < b.first.arrivalTime;
        });

    result.metrics.resize(n);

    int currentTime = 0;

    for (const auto& item : order)
    {
        Process p = item.first;
        int originalIndex = item.second;

        // Handle CPU idle time
        if (currentTime < p.arrivalTime)
        {
            result.ganttChart.push_back({
                -1, currentTime, p.arrivalTime
            });

            currentTime = p.arrivalTime;
        }

        int startTime = currentTime;

        // Record why this process was selected
        string reason = "Selected by FCFS based on arrival order.";

        result.decisions.push_back({
            currentTime, p.pid, reason
        });

        // Execute process until completion
        currentTime += p.burstTime;

        int completionTime = currentTime;
        int turnaroundTime = completionTime - p.arrivalTime;
        int waitingTime = turnaroundTime - p.burstTime;
        int responseTime = startTime - p.arrivalTime;

        // Store Gantt chart segment
        result.ganttChart.push_back({
            p.pid, startTime, completionTime
        });

        // Store process metrics
        result.metrics[originalIndex] = {
            p.pid,
            completionTime,
            turnaroundTime,
            waitingTime,
            responseTime
        };

        result.averageTurnaroundTime += turnaroundTime;
        result.averageWaitingTime += waitingTime;
        result.averageResponseTime += responseTime;
    }

    // Calculate averages
    result.averageTurnaroundTime /= n;
    result.averageWaitingTime /= n;
    result.averageResponseTime /= n;

    return result;
}



SchedulingResult runSJF(const vector<Process>& processes)
{
    SchedulingResult result;

    result.averageTurnaroundTime = 0;
    result.averageWaitingTime = 0;
    result.averageResponseTime = 0;

    int n = processes.size();

    if (n == 0)
        return result;

    result.metrics.resize(n);

    vector<bool> completed(n, false);

    int completedCount = 0;
    int currentTime = 0;

    while (completedCount < n)
    {
        int selected = -1;

        // Find the shortest burst among arrived processes
        for (int i = 0; i < n; i++)
        {
            if (!completed[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                if (selected == -1 ||
                    processes[i].burstTime <
                    processes[selected].burstTime)
                {
                    selected = i;
                }
            }
        }

        // No process is ready: CPU remains idle
        if (selected == -1)
        {
            int nextArrival = -1;

            for (int i = 0; i < n; i++)
            {
                if (!completed[i])
                {
                    if (nextArrival == -1 ||
                        processes[i].arrivalTime <
                        nextArrival)
                    {
                        nextArrival = processes[i].arrivalTime;
                    }
                }
            }

            result.ganttChart.push_back({
                -1, currentTime, nextArrival
            });

            currentTime = nextArrival;
            continue;
        }

        Process p = processes[selected];

        int startTime = currentTime;

        result.decisions.push_back({
            currentTime,
            p.pid,
            "Selected because it has the shortest burst time among ready processes."
        });

        // Execute until completion
        currentTime += p.burstTime;

        int ct = currentTime;
        int tat = ct - p.arrivalTime;
        int wt = tat - p.burstTime;
        int rt = startTime - p.arrivalTime;

        result.ganttChart.push_back({
            p.pid, startTime, ct
        });

        result.metrics[selected] = {
            p.pid, ct, tat, wt, rt
        };

        result.averageTurnaroundTime += tat;
        result.averageWaitingTime += wt;
        result.averageResponseTime += rt;

        completed[selected] = true;
        completedCount++;
    }

    result.averageTurnaroundTime /= n;
    result.averageWaitingTime /= n;
    result.averageResponseTime /= n;

    return result;
}


SchedulingResult runSRTF(const vector<Process>& processes)
{
    SchedulingResult result;

    result.averageTurnaroundTime = 0;
    result.averageWaitingTime = 0;
    result.averageResponseTime = 0;

    int n = processes.size();

    if (n == 0)
        return result;

    vector<int> remainingTime(n);
    vector<int> firstStart(n, -1);
    vector<int> completionTime(n, 0);

    for (int i = 0; i < n; i++)
        remainingTime[i] = processes[i].burstTime;

    int currentTime = 0;
    int completedCount = 0;
    int previousProcess = -1;

    result.metrics.resize(n);

    while (completedCount < n)
    {
        int selected = -1;

        // Select the process with the shortest remaining time
        for (int i = 0; i < n; i++)
        {
            if (remainingTime[i] > 0 &&
                processes[i].arrivalTime <= currentTime)
            {
                if (selected == -1 ||
                    remainingTime[i] < remainingTime[selected])
                {
                    selected = i;
                }
                else if (remainingTime[i] == remainingTime[selected])
                {
                    // On a tie, continue the current process if possible
                    if (i == previousProcess)
                        selected = i;
                }
            }
        }

        // No process is ready: CPU is idle
        if (selected == -1)
        {
            if (!result.ganttChart.empty() &&
                result.ganttChart.back().pid == -1)
            {
                result.ganttChart.back().endTime++;
            }
            else
            {
                result.ganttChart.push_back({
                    -1, currentTime, currentTime + 1
                });
            }

            currentTime++;
            previousProcess = -1;
            continue;
        }

        Process p = processes[selected];

        // Record the first time this process gets CPU
        if (firstStart[selected] == -1)
            firstStart[selected] = currentTime;

        // Record a decision whenever the running process changes
        if (selected != previousProcess)
        {
            string reason;

            if (previousProcess != -1 &&
                remainingTime[previousProcess] > 0)
            {
                reason = "Preempted P" +
                    to_string(processes[previousProcess].pid) +
                    " because this process has a shorter remaining time.";
            }
            else
            {
                reason = "Selected because it has the shortest remaining time among ready processes.";
            }

            result.decisions.push_back({
                currentTime, p.pid, reason
            });
        }

        // Execute for one time unit
        if (!result.ganttChart.empty() &&
            result.ganttChart.back().pid == p.pid &&
            result.ganttChart.back().endTime == currentTime)
        {
            result.ganttChart.back().endTime++;
        }
        else
        {
            result.ganttChart.push_back({
                p.pid, currentTime, currentTime + 1
            });
        }

        remainingTime[selected]--;
        currentTime++;

        // Check whether the process has completed
        if (remainingTime[selected] == 0)
        {
            completionTime[selected] = currentTime;
            completedCount++;
        }

        previousProcess = selected;
    }

    // Calculate metrics
    for (int i = 0; i < n; i++)
    {
        int ct = completionTime[i];
        int tat = ct - processes[i].arrivalTime;
        int wt = tat - processes[i].burstTime;
        int rt = firstStart[i] - processes[i].arrivalTime;

        result.metrics[i] = {
            processes[i].pid, ct, tat, wt, rt
        };

        result.averageTurnaroundTime += tat;
        result.averageWaitingTime += wt;
        result.averageResponseTime += rt;
    }

    result.averageTurnaroundTime /= n;
    result.averageWaitingTime /= n;
    result.averageResponseTime /= n;

    return result;
}


SchedulingResult runPriority(const vector<Process>& processes)
{
    SchedulingResult result;

    result.averageTurnaroundTime = 0;
    result.averageWaitingTime = 0;
    result.averageResponseTime = 0;

    int n = processes.size();

    if (n == 0)
        return result;

    vector<bool> completed(n, false);
    result.metrics.resize(n);

    int currentTime = 0;
    int completedCount = 0;

    while (completedCount < n)
    {
        int selected = -1;

        // Find the highest-priority arrived process
        // Lower priority number means higher priority
        for (int i = 0; i < n; i++)
        {
            if (!completed[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                if (selected == -1 ||
                    processes[i].priority <
                    processes[selected].priority)
                {
                    selected = i;
                }
            }
        }

        // No process is ready: CPU is idle
        if (selected == -1)
        {
            int nextArrival = -1;

            for (int i = 0; i < n; i++)
            {
                if (!completed[i])
                {
                    if (nextArrival == -1 ||
                        processes[i].arrivalTime < nextArrival)
                    {
                        nextArrival = processes[i].arrivalTime;
                    }
                }
            }

            result.ganttChart.push_back({
                -1, currentTime, nextArrival
            });

            currentTime = nextArrival;
            continue;
        }

        Process p = processes[selected];

        int startTime = currentTime;

        result.decisions.push_back({
            currentTime,
            p.pid,
            "Selected because it has the highest priority among ready processes."
        });

        // Execute until completion
        currentTime += p.burstTime;

        int ct = currentTime;
        int tat = ct - p.arrivalTime;
        int wt = tat - p.burstTime;
        int rt = startTime - p.arrivalTime;

        result.ganttChart.push_back({
            p.pid, startTime, ct
        });

        result.metrics[selected] = {
            p.pid, ct, tat, wt, rt
        };

        result.averageTurnaroundTime += tat;
        result.averageWaitingTime += wt;
        result.averageResponseTime += rt;

        completed[selected] = true;
        completedCount++;
    }

    result.averageTurnaroundTime /= n;
    result.averageWaitingTime /= n;
    result.averageResponseTime /= n;

    return result;
}


SchedulingResult runRoundRobin(
    const vector<Process>& processes,
    int timeQuantum)
{
    SchedulingResult result;

    result.averageTurnaroundTime = 0;
    result.averageWaitingTime = 0;
    result.averageResponseTime = 0;

    int n = processes.size();

    if (n == 0 || timeQuantum <= 0)
        return result;

    vector<pair<Process, int>> order;

    for (int i = 0; i < n; i++)
        order.push_back({processes[i], i});

    stable_sort(order.begin(), order.end(),
        [](const pair<Process, int>& a,
           const pair<Process, int>& b)
        {
            return a.first.arrivalTime < b.first.arrivalTime;
        });

    vector<int> remainingTime(n);
    vector<int> firstStart(n, -1);
    vector<int> completionTime(n, 0);
    vector<string> dispatchReason(n);

    for (int i = 0; i < n; i++)
        remainingTime[i] = processes[i].burstTime;

    queue<int> readyQueue;

    int currentTime = 0;
    int completedCount = 0;
    int nextArrival = 0;

    result.metrics.resize(n);

    while (completedCount < n)
    {
        // Add all processes that have arrived
        while (nextArrival < n &&
               order[nextArrival].first.arrivalTime <= currentTime)
        {
            int index = order[nextArrival].second;

            readyQueue.push(index);
            dispatchReason[index] = "Process arrived and entered the ready queue.";

            nextArrival++;
        }

        // If queue is empty, CPU is idle
        if (readyQueue.empty())
        {
            int idleStart = currentTime;
            currentTime = order[nextArrival].first.arrivalTime;

            result.ganttChart.push_back({
                -1, idleStart, currentTime
            });

            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        Process p = processes[index];

        if (firstStart[index] == -1)
            firstStart[index] = currentTime;

        // Record scheduling decision
        result.decisions.push_back({
            currentTime,
            p.pid,
            dispatchReason[index]
        });

        // Execute for one time quantum or until completion
        int executionTime = min(timeQuantum, remainingTime[index]);

        int startTime = currentTime;
        currentTime += executionTime;
        remainingTime[index] -= executionTime;

        // Add Gantt chart segment
        if (!result.ganttChart.empty() &&
            result.ganttChart.back().pid == p.pid &&
            result.ganttChart.back().endTime == startTime)
        {
            result.ganttChart.back().endTime = currentTime;
        }
        else
        {
            result.ganttChart.push_back({
                p.pid, startTime, currentTime
            });
        }

        // Add newly arrived processes before re-queuing
        // the process whose time quantum just expired
        while (nextArrival < n &&
               order[nextArrival].first.arrivalTime <= currentTime)
        {
            int newIndex = order[nextArrival].second;

            readyQueue.push(newIndex);
            dispatchReason[newIndex] =
                "Process arrived and entered the ready queue.";

            nextArrival++;
        }

        if (remainingTime[index] > 0)
        {
            readyQueue.push(index);

            dispatchReason[index] =
                "Time quantum expired; process returned to the end of the ready queue.";
        }
        else
        {
            completionTime[index] = currentTime;
            completedCount++;
        }
    }

    // Calculate process metrics
    for (int i = 0; i < n; i++)
    {
        int ct = completionTime[i];
        int tat = ct - processes[i].arrivalTime;
        int wt = tat - processes[i].burstTime;
        int rt = firstStart[i] - processes[i].arrivalTime;

        result.metrics[i] = {
            processes[i].pid, ct, tat, wt, rt
        };

        result.averageTurnaroundTime += tat;
        result.averageWaitingTime += wt;
        result.averageResponseTime += rt;
    }

    result.averageTurnaroundTime /= n;
    result.averageWaitingTime /= n;
    result.averageResponseTime /= n;

    return result;
}
