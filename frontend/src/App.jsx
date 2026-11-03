
import CPUScheduling from "./CPUScheduling";
import DecisionAnalyzer from "./DecisionAnalyzer";
import MemoryManagement from "./MemoryManagement";
import DeadlockManagement from "./DeadlockManagement";
import ProcessManagement from "./ProcessManagement";
import { useState } from "react";
import "./App.css";
import "./DesignSystem.css";

const modules = [
  { id: "Dashboard", icon: "▦", label: "Dashboard" },
  { id: "Process Management", icon: "◉", label: "Process Management" },
  { id: "CPU Scheduling", icon: "▤", label: "CPU Scheduling" },
  { id: "Decision Analyzer", icon: "⌁", label: "Decision Analyzer" },
  { id: "Memory Management", icon: "▣", label: "Memory Management" },
  { id: "Deadlock Management", icon: "⛨", label: "Deadlock Management" },
];

function App() {
  const [activePage, setActivePage] = useState("Dashboard");

  return (
    <div className="app-container">
      <aside className="sidebar">
        <div className="brand">
          <div className="brand-icon">OS</div>
          <div>
            <h2>Resource Manager</h2>
            <span>Operating System Simulator</span>
          </div>
        </div>

        <div className="nav-heading">MAIN MENU</div>

        <nav className="navigation">
          {modules.map((module) => (
            <button
              key={module.id}
              className={`nav-item ${
                activePage === module.id ? "active" : ""
              }`}
              onClick={() => setActivePage(module.id)}
            >
              <span className="nav-icon">{module.icon}</span>
              <span>{module.label}</span>
            </button>
          ))}
        </nav>

        <div className="sidebar-footer">
          <div className="status-dot"></div>
          <span>System Simulator Online</span>
        </div>
      </aside>

      <main className="main-content">
        <header className="topbar">
          <div>
            <p className="breadcrumb">OS SIMULATOR / {activePage.toUpperCase()}</p>
            <h1>{activePage}</h1>
          </div>

          <div className="topbar-right">
            <span className="system-status">
              <span className="status-dot"></span>
              System Ready
            </span>
            <div className="profile">OS</div>
          </div>
        </header>

        {activePage === "Process Management" ? (
          <ProcessManagement />
        ) : activePage === "CPU Scheduling" ? (
          <CPUScheduling />
        ) : activePage === "Decision Analyzer" ? (
          <DecisionAnalyzer />
        ) : activePage === "Memory Management" ? (
          <MemoryManagement /> 
        ) : activePage === "Deadlock Management" ? (
          <DeadlockManagement />   
        ) : activePage === "Dashboard" ? (
          <section className="dashboard">
            <div className="welcome-section">
              <div>
                <h2>System Overview</h2>
                <p>
                  Monitor processes, CPU scheduling, memory and system resources.
                </p>
              </div>
              <span className="overview-tag">SIMULATION ENVIRONMENT</span>
            </div>

            <div className="stats-grid">
              <div className="stat-card">
                <div className="stat-top">
                  <span>Total Processes</span>
                  <span className="stat-icon purple">◉</span>
                </div>
                <h2>0</h2>
                <p>Processes created</p>
              </div>

              <div className="stat-card">
                <div className="stat-top">
                  <span>CPU Utilization</span>
                  <span className="stat-icon blue">▤</span>
                </div>
                <h2>0%</h2>
                <p>Processor usage</p>
              </div>

              <div className="stat-card">
                <div className="stat-top">
                  <span>Memory Usage</span>
                  <span className="stat-icon green">▣</span>
                </div>
                <h2>0%</h2>
                <p>Memory allocated</p>
              </div>

              <div className="stat-card">
                <div className="stat-top">
                  <span>Deadlock Status</span>
                  <span className="stat-icon orange">⛨</span>
                </div>
                <h2 className="safe-text">Safe</h2>
                <p>No active simulation</p>
              </div>
            </div>

            <div className="content-grid">
              <div className="panel">
                <div className="panel-heading">
                  <div>
                    <h3>System Modules</h3>
                    <p>Access the different components of the simulator.</p>
                  </div>
                </div>

                <div className="module-grid">
                  {modules.slice(1).map((module) => (
                    <button
                      className="module-card"
                      key={module.id}
                      onClick={() => setActivePage(module.id)}
                    >
                      <span className="module-icon">{module.icon}</span>
                      <span>{module.label}</span>
                      <span className="module-arrow">→</span>
                    </button>
                  ))}
                </div>
              </div>

              <div className="panel activity-panel">
                <div className="panel-heading">
                  <div>
                    <h3>System Activity</h3>
                    <p>Latest simulation activity</p>
                  </div>
                </div>

                <div className="empty-state">
                  <div className="empty-icon">⌁</div>
                  <h4>No activity yet</h4>
                  <p>
                    Run a simulation to see system activity and results here.
                  </p>
                </div>
              </div>
            </div>
          </section>
        ) : (
          <section className="placeholder-panel">
            <div className="placeholder-icon">
              {modules.find((module) => module.id === activePage)?.icon}
            </div>
            <h2>{activePage}</h2>
            <p>
              This module will be implemented next. Select Dashboard to return
              to the overview.
            </p>
            <button onClick={() => setActivePage("Dashboard")}>
              Back to Dashboard
            </button>
          </section>
        )}
      </main>
    </div>
  );
}

export default App;
