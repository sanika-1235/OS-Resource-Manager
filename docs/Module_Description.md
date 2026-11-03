# Module Description

This document summarizes the main functional modules of the OS Resource Manager Simulator.

## 1. Process Management

**Source location:** `backend/process/`  
**Frontend location:** `frontend/src/ProcessManagement.jsx`

The Process Management module provides process lifecycle operations through the dashboard:

- Adding processes.
- Updating process states.
- Terminating processes.
- Viewing process information and state.

The process manager validates state transitions. Termination is supported from NEW, READY, RUNNING, and WAITING states.

## 2. CPU Scheduling

**Source location:** `backend/cpu/`  
**Frontend location:** `frontend/src/CPUScheduling.jsx`

The CPU Scheduling module implements five scheduling algorithms:

1. First Come First Serve (FCFS)
2. Shortest Job First (SJF)
3. Shortest Remaining Time First (SRTF)
4. Priority Scheduling
5. Round Robin (RR)

The interface allows users to run scheduling simulations and compare results. The backend scheduling implementation is in `Scheduler.cpp` and `Scheduler.h`; decision analysis is also implemented in the CPU module.

## 3. Memory Management

**Source location:** `backend/memory/`  
**Frontend location:** `frontend/src/MemoryManagement.jsx`

The Memory Management module provides memory-related simulation functionality. The backend contains:

- Memory allocation (`allocation.cpp`)
- Page replacement (`page_replacement.cpp`)

The frontend includes controls for memory allocation and resetting the simulation.

## 4. Deadlock Management

**Source location:** `backend/deadlock/`  
**Frontend location:** `frontend/src/DeadlockManagement.jsx`

The Deadlock Management module monitors resource allocation and handles resource requests.

The interface provides:

- A resource allocation table showing allocation, maximum requirement, and remaining need.
- Available resource information.
- Resource requests for selected processes.
- A safety check using the Banker's safety algorithm.
- A simulation reset operation.

The safety check reports whether the current resource state is safe and may display a safe sequence.

## 5. OS Decision Analyzer

**Backend location:** `backend/cpu/DecisionAnalyzer.cpp`  
**Frontend location:** `frontend/src/DecisionAnalyzer.jsx`

The OS Decision Analyzer presents decision-analysis functionality through the frontend and uses the backend decision-analyzer implementation. For exact inputs, outputs, and decision rules, consult the implementation and corresponding UI.

## Functional Verification

The following main workflows were manually exercised during integration testing:

| Module | Workflows checked |
|---|---|
| Process Management | Add process, update state, terminate process |
| CPU Scheduling | Run all five scheduling algorithms |
| Memory Management | Allocate memory, reset memory |
| Deadlock Management | Run safety check, submit resource request, reset simulation |
| Application | Restart frontend/backend and check that modules load |

The browser console was also checked for visible red errors during the verification session.

## Scope and Limitations

This document records the functionality exercised during the project integration check. It does not claim that every edge case, performance condition, or failure scenario has been exhaustively tested.
