
#include <iostream>
#include <cassert>
#include "../backend/process/ProcessManager.h"

using namespace std;

int main() {
    ProcessManager manager;

    cout << "\n===== PROCESS MANAGEMENT TESTS =====\n";

    // Test 1: Valid process creation
    assert(manager.createProcess(101, "ProcessA", 0, 10, 1));
    cout << "Test 1 PASSED: Valid process creation\n";

    // Test 2: Duplicate PID
    assert(!manager.createProcess(101, "ProcessB", 1, 5, 2));
    cout << "Test 2 PASSED: Duplicate PID rejected\n";

    // Test 3: Invalid burst time
    assert(!manager.createProcess(102, "ProcessC", 0, 0, 1));
    cout << "Test 3 PASSED: Invalid burst time rejected\n";

    // Test 4: Invalid arrival time
    assert(!manager.createProcess(103, "ProcessD", -1, 5, 1));
    cout << "Test 4 PASSED: Invalid arrival time rejected\n";

    // Test 5: Invalid priority
    assert(!manager.createProcess(104, "ProcessE", 0, 5, -1));
    cout << "Test 5 PASSED: Invalid priority rejected\n";

    // Test 6: Valid transitions
    assert(manager.updateState(101, ProcessState::READY));
    assert(manager.updateState(101, ProcessState::RUNNING));
    assert(manager.updateState(101, ProcessState::WAITING));
    assert(manager.updateState(101, ProcessState::READY));
    cout << "Test 6 PASSED: Valid state transitions\n";

    // Test 7: Invalid transition (READY to WAITING)
    assert(!manager.updateState(101, ProcessState::WAITING));
    cout << "Test 7 PASSED: Invalid transition rejected\n";

    // Test 8: Termination and restart prevention
    assert(manager.updateState(101, ProcessState::RUNNING));
    assert(manager.terminateProcess(101));
    assert(!manager.updateState(101, ProcessState::READY));
    cout << "Test 8 PASSED: Terminated process cannot restart\n";

    cout << "\n===== ALL TESTS PASSED =====\n";

    return 0;
}
