# System Architecture

## Overview

The OS Resource Manager Simulator is a web-based academic project with a React frontend and a C++ backend. The frontend provides the user interface for interacting with operating-system simulations. The backend contains the simulation logic and exposes HTTP endpoints that the frontend uses to exchange data in JSON format.

## High-Level Architecture

```text
User
 |
 v
React Frontend (Vite)
 |
 | HTTP requests / JSON responses
 v
C++ Backend API
 |
 +--> Process Management
 +--> CPU Scheduling
 +--> Memory Management
 +--> Deadlock & Resource Management
 +--> OS Decision Analyzer
```

## Main Components

### Frontend

Location: `frontend/`

Built with React and Vite, the frontend provides the simulator dashboard and interfaces for Process Management, CPU Scheduling, Memory Management, Deadlock Management, and the OS Decision Analyzer.

### Backend

Location: `backend/`

The C++ backend receives frontend requests through its API layer and connects them to the relevant module logic.

- `backend/api/` — API/server entry point.
- `backend/process/` — process management.
- `backend/cpu/` — CPU scheduling and decision analysis.
- `backend/memory/` — memory allocation and page replacement.
- `backend/deadlock/` — deadlock and resource management.

## Communication Flow

1. A user performs an action in the React interface.
2. The frontend sends an HTTP request to the backend.
3. The backend processes the request using the relevant C++ module.
4. The backend returns a JSON response.
5. The frontend displays the result.

## Technology Stack

| Layer | Technology |
|---|---|
| Frontend | React.js |
| Frontend tooling | Vite |
| Backend | C++ |
| C++ standard | C++17 |
| Communication | HTTP and JSON |
| Compiler used during development | GCC (MinGW-w64) |

## Running the Application

Refer to the repository-root `README.md` for the current installation, compilation, and run commands. Run the frontend and backend in separate terminals.

## Notes

This document describes the high-level architecture. For exact endpoint names, request schemas, and implementation details, refer to the source code in `backend/api/` and the corresponding module files.
