# OS Resource Manager Simulator

An interactive Operating System Resource Manager Simulator designed to demonstrate and visualize core operating system concepts, including process management, CPU scheduling, memory management, and deadlock handling.

The project combines a **C++ backend** with a **React-based frontend** to provide an interactive dashboard for exploring operating system resource management concepts.

## Features

### 1. Process Management
- Create and manage processes.
- Update process states.
- Terminate processes.
- Track process information and execution states.

### 2. CPU Scheduling
Supports five CPU scheduling algorithms:
- First Come First Serve (FCFS)
- Shortest Job First (SJF)
- Shortest Remaining Time First (SRTF)
- Priority Scheduling
- Round Robin (RR)

Compare scheduling results and analyze execution performance.

### 3. Memory Management
- Dynamic memory allocation.
- Monitor available memory resources.
- Reset memory allocation state.

### 4. Deadlock Management
- Resource allocation monitoring.
- Resource request handling.
- Banker's Safety Algorithm.
- Safe and unsafe state analysis.
- Reset simulation to initial resource values.

## Technology Stack

| Component | Technology |
|---|---|
| Frontend | React.js |
| Development Server | Vite |
| Backend | C++ |
| Programming Standard | C++17 |
| Communication | HTTP / JSON |
| Compiler | GCC (MinGW-w64) |

## Project Structure

```text
OS-Resource-Manager/
│
├── backend/
│   ├── api/
│   ├── process/
│   ├── cpu/
│   ├── memory/
│   └── deadlock/
│
├── frontend/
│   ├── src/
│   ├── public/
│   └── package.json
│
└── README.md
```

## Getting Started

### Prerequisites

- Node.js and npm
- GCC compiler with C++17 support
- Git
- Visual Studio Code (recommended)

### 1. Clone the Repository

```bash
git clone <https://github.com/sanika-1235/OS-Resource-Manager.git>
cd OS-Resource-Manager
```

### 2. Install Frontend Dependencies

```bash
cd frontend
npm install
```

### 3. Start the Frontend

```bash
npm run dev
```

Open the local URL displayed by Vite, usually:

```text
http://localhost:5173
```

### 4. Compile the Backend

Open a separate terminal in the project root and run:

```powershell
g++ -std=c++17 -pthread backend/api/main.cpp backend/process/Process.cpp backend/process/ProcessManager.cpp backend/cpu/Scheduler.cpp backend/cpu/DecisionAnalyzer.cpp backend/deadlock/DeadlockManager.cpp backend/memory/allocation.cpp backend/memory/page_replacement.cpp -o backend_test.exe -lws2_32
```

### 5. Run the Backend

```powershell
.\backend_test.exe
```

The backend server runs on:

```text
http://localhost:8080
```

Keep both frontend and backend terminals running while using the simulator.

## Testing

The following functional workflows were tested:

- Process creation, state updates and termination.
- All five CPU scheduling algorithms.
- Memory allocation and reset.
- Deadlock safety analysis and resource requests.
- Simulation reset and restoration of initial resource values.
- Frontend and backend restart verification.

## Learning Objectives

This project demonstrates practical implementation of:

- Process lifecycle management.
- CPU scheduling strategies.
- Memory allocation techniques.
- Resource allocation and deadlock prevention concepts.
- Client-server communication.
- Integration of frontend interfaces with backend logic.

## Contributors

Developed as a team academic project as part of the Operating Systems course.

**Team Members:**  
- [NAMALA NAVYA SREE - E3NG24CS0138]
- [PALLAVI V PATIL - ENG24CS0157]
- [PRIYANKA S - ENG24CS0169]
- [SANIKA K S - ENG24CS0203]

---

*Built to make Operating System concepts interactive, practical, and easier to understand.*
