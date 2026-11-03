
#include "backend/cpu/Scheduler.h"
#include <iostream>
#include <iomanip>
#include "backend/cpu/DecisionAnalyzer.h"

int main()
{
    vector<Process> processes = {
    {1,0,5,2}, {2,0,3,1}, {3,0,1,3}};
    SchedulingResult result = runPriority(processes);
    displayDecisionLog(result);
    displayWaitingAnalysis(result);

    cout << "GANTT CHART\n";

    for (const auto& segment : result.ganttChart)
    {
        if (segment.pid == -1)
            cout << "| IDLE ";
        else
            cout << "| P" << segment.pid << " ";

        cout << "(" << segment.startTime
             << "-" << segment.endTime << ") ";
    }

    cout << "|\n\n";

    cout << "PID\tCT\tTAT\tWT\tRT\n";

    for (const auto& m : result.metrics)
    {
        cout << m.pid << "\t"
             << m.completionTime << "\t"
             << m.turnaroundTime << "\t"
             << m.waitingTime << "\t"
             << m.responseTime << "\n";
    }

    cout << fixed << setprecision(2);

    cout << "\nAverage TAT: "
         << result.averageTurnaroundTime;

    cout << "\nAverage WT: "
         << result.averageWaitingTime;

    cout << "\nAverage RT: "
         << result.averageResponseTime << "\n";

    return 0;
}
