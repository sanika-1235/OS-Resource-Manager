# Frontend — OS Resource Manager Simulator

This folder contains the React-based user interface for the OS Resource Manager Simulator.

The frontend communicates with the C++ backend through HTTP APIs and provides an interactive dashboard for managing and visualizing operating system concepts.

## Technology Stack

- React.js
- Vite
- JavaScript
- CSS
- HTTP / JSON

## Features

- Process Management dashboard
- CPU Scheduling visualization and algorithm comparison
- Memory Management interface
- Deadlock Management and safety analysis
- OS Decision Analyzer interface

## Getting Started

### Prerequisites

- Node.js
- npm

### 1. Install Dependencies

From this directory, run:

```bash
npm install
```

### 2. Start Development Server

```bash
npm run dev
```

The application will usually be available at:

```text
http://localhost:5173
```

### 3. Build for Production

```bash
npm run build
```

### 4. Preview Production Build

```bash
npm run preview
```

## Project Structure

```text
frontend/
├── public/
├── src/
├── index.html
├── package.json
├── package-lock.json
├── vite.config.js
└── README.md
```

## Backend Connection

The frontend requires the C++ backend to be running separately.

Refer to the main project README in the repository root for backend compilation and execution instructions.

---

Part of the OS Resource Manager Simulator academic team project.
