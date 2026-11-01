
#include "DecisionAnalyzer.h"
#include <iostream>

using namespace std;

// Display scheduling decisions
void displayDecisionLog(const SchedulingResult& result)
{
    cout << "\n===== OS DECISION ANALYZER =====\n";

    if (result.decisions.empty())
    {
        cout << "No scheduling decisions recorded.\n";
        return;
    }

    for (const auto& decision : result.decisions)
    {
        cout << "\nTime: " << decision.time;
        cout << "\nSelected Process: P" << decision.selectedPid;
        cout << "\nReason: " << decision.reason << "\n";
    }
}

// Explain process waiting times
void displayWaitingAnalysis(const SchedulingResult& result)
{
    cout << "\n===== PROCESS WAITING ANALYSIS =====\n";

    if (result.metrics.empty())
    {
        cout << "No process metrics available.\n";
        return;
    }

    for (const auto& metric : result.metrics)
    {
        cout << "\nProcess P" << metric.pid
             << " waited for "
             << metric.waitingTime
             << " time units in the ready queue.\n";
    }

    cout << "\nAverage Waiting Time: "
         << result.averageWaitingTime << "\n";
}
