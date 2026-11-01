
#ifndef DECISION_ANALYZER_H
#define DECISION_ANALYZER_H

#include "Scheduler.h"

// Display why each process was selected
void displayDecisionLog(const SchedulingResult& result);

// Explain how long each process waited
void displayWaitingAnalysis(const SchedulingResult& result);

#endif
