import { useState } from "react";
import "./CPUScheduling.css";

const initialProcesses = [
  { pid: 1, arrivalTime: 0, burstTime: 5, priority: 2 },
  { pid: 2, arrivalTime: 1, burstTime: 3, priority: 1 },
  { pid: 3, arrivalTime: 2, burstTime: 1, priority: 3 },
];

const algorithms = ["FCFS", "SJF", "SRTF", "Priority", "Round Robin"];

function CPUScheduling() {
  const [algorithm, setAlgorithm] = useState("FCFS");
  const [processes, setProcesses] = useState(initialProcesses);
  const [quantum, setQuantum] = useState(2);
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");

  const clearResults = () => {
    setResult(null);
    setError("");
  };

  const updateProcess = (index, field, value) => {
    const updated = [...processes];
    updated[index] = { ...updated[index], [field]: value };
    setProcesses(updated);
    clearResults();
  };

  const addProcess = () => {
    const nextId = processes.length
      ? Math.max(...processes.map((p) => p.pid)) + 1
      : 1;

    setProcesses([
      ...processes,
      {
        pid: nextId,
        arrivalTime: 0,
        burstTime: 1,
        priority: 1,
      },
    ]);

    clearResults();
  };

  const removeProcess = (pid) => {
    setProcesses(processes.filter((p) => p.pid !== pid));
    clearResults();
  };

  const runSimulation = async () => {
    const invalidProcesses = processes.some(
      (p) =>
        !Number.isInteger(p.pid) ||
        !Number.isInteger(p.arrivalTime) ||
        !Number.isInteger(p.burstTime) ||
        !Number.isInteger(p.priority) ||
        p.pid <= 0 ||
        p.arrivalTime < 0 ||
        p.burstTime <= 0 ||
        p.priority < 0
    );

    const duplicatePids =
      new Set(processes.map((p) => p.pid)).size !== processes.length;

    if (processes.length === 0 || invalidProcesses || duplicatePids) {
      setError(
        "Enter valid process values. Process IDs must be unique and positive, and burst time must be positive."
      );
      setResult(null);
      return;
    }

    if (
      algorithm === "Round Robin" &&
      (!Number.isInteger(quantum) || quantum <= 0)
    ) {
      setError("Time quantum must be a positive integer.");
      setResult(null);
      return;
    }

    setLoading(true);
    setError("");
    setResult(null);

    try {
      const requestBody = {
        algorithm,
        processes,
        timeQuantum: quantum,
      };

      const response = await fetch(
        "http://localhost:8080/api/scheduling/run",
        {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify(requestBody),
        }
      );

      const data = await response.json();

      if (!response.ok) {
        throw new Error(data.error || data.message || "Simulation failed.");
      }

      if (
        !Array.isArray(data.metrics) ||
        !Array.isArray(data.ganttChart) ||
        !Array.isArray(data.decisions)
      ) {
        throw new Error("The backend returned an unexpected response.");
      }

      setResult(data);
    } catch (err) {
      console.error("CPU scheduling error:", err);
      setError(
        err.message ||
          "Unable to connect to the backend. Make sure the C++ server is running on port 8080."
      );
    } finally {
      setLoading(false);
    }
  };

  const ganttChart = result?.ganttChart || [];

  const makespan = ganttChart.reduce(
    (maximum, segment) => Math.max(maximum, segment.endTime),
    0
  );

  const busyTime = ganttChart.reduce((total, segment) => {
    return segment.pid > 0
      ? total + (segment.endTime - segment.startTime)
      : total;
  }, 0);

  const cpuUtilization =
    makespan > 0 ? (busyTime / makespan) * 100 : 0;

  const getGanttClass = (pid) => {
    if (pid <= 0) return "block-idle";
    return `block-color-${(pid - 1) % 6}`;
  };

  return (
    <div className="cpu-page">
      <div className="cpu-intro">
        <div>
          <h2>CPU Scheduling Simulator</h2>
          <p>
            Configure processes, choose a scheduling algorithm and analyze
            CPU performance.
          </p>
        </div>
        <span className="cpu-tag">CPU MODULE</span>
      </div>

      <section className="cpu-panel">
        <div className="cpu-panel-heading">
          <div>
            <h3>Scheduling Algorithm</h3>
            <p>Select an algorithm to run the simulation.</p>
          </div>
        </div>

        <div className="algorithm-options">
          {algorithms.map((item) => (
            <button
              key={item}
              className={`algorithm-button ${
                algorithm === item ? "selected" : ""
              }`}
              onClick={() => {
                setAlgorithm(item);
                clearResults();
              }}
              disabled={loading}
            >
              {item}
            </button>
          ))}
        </div>

        {algorithm === "Round Robin" && (
          <div className="quantum-input">
            <label>Time Quantum</label>
            <input
              type="number"
              min="1"
              value={quantum}
              onChange={(e) => {
                setQuantum(Number(e.target.value));
                clearResults();
              }}
              disabled={loading}
            />
          </div>
        )}
      </section>

      <section className="cpu-panel process-panel">
        <div className="cpu-panel-heading process-heading">
          <div>
            <h3>Process Configuration</h3>
            <p>Add and configure processes for scheduling.</p>
          </div>

          <button
            className="add-process"
            onClick={addProcess}
            disabled={loading}
          >
            + Add Process
          </button>
        </div>

        <div className="table-scroll">
          <table className="process-table">
            <thead>
              <tr>
                <th>Process ID</th>
                <th>Arrival Time</th>
                <th>Burst Time</th>
                <th>Priority</th>
                <th>Action</th>
              </tr>
            </thead>

            <tbody>
              {processes.map((process, index) => (
                <tr key={process.pid}>
                  <td>
                    <span className="pid-badge">P{process.pid}</span>
                  </td>

                  {["arrivalTime", "burstTime", "priority"].map((field) => (
                    <td key={field}>
                      <input
                        className="process-input"
                        type="number"
                        min={field === "burstTime" ? "1" : "0"}
                        value={process[field]}
                        disabled={loading}
                        onChange={(e) =>
                          updateProcess(
                            index,
                            field,
                            e.target.value === ""
                              ? ""
                              : Number(e.target.value)
                          )
                        }
                      />
                    </td>
                  ))}

                  <td>
                    <button
                      className="remove-process"
                      onClick={() => removeProcess(process.pid)}
                      aria-label={`Remove process ${process.pid}`}
                      disabled={loading}
                    >
                      ×
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        <div className="simulation-actions">
          <span>{processes.length} processes configured</span>

          <button
            className="run-simulation"
            onClick={runSimulation}
            disabled={loading}
          >
            {loading ? "Running..." : "▶ Run Simulation"}
          </button>
        </div>
      </section>

      {error && (
        <div className="cpu-error" role="alert">
          <strong>Simulation Error</strong>
          <p>{error}</p>
        </div>
      )}

      {loading && (
        <div className="cpu-loading">
          Running {algorithm} scheduling simulation...
        </div>
      )}

      {result && (
        <section className="cpu-results">
          <div className="results-heading">
            <div>
              <h3>Scheduling Results</h3>
              <p>
                {algorithm}
                {algorithm === "Round Robin" ? ` · Quantum ${quantum}` : ""}
                {" · Actual backend results"}
              </p>
            </div>
            <span className="result-badge success-badge">
              Simulation complete
            </span>
          </div>

          <div className="result-stats">
            <div className="result-stat">
              <span>Average Turnaround Time</span>
              <h2>{result.averageTurnaroundTime.toFixed(2)}</h2>
              <small>Time units</small>
            </div>

            <div className="result-stat">
              <span>Average Waiting Time</span>
              <h2>{result.averageWaitingTime.toFixed(2)}</h2>
              <small>Time units</small>
            </div>

            <div className="result-stat">
              <span>Average Response Time</span>
              <h2>{result.averageResponseTime.toFixed(2)}</h2>
              <small>Time units</small>
            </div>

            <div className="result-stat">
              <span>CPU Utilization</span>
              <h2>{cpuUtilization.toFixed(2)}%</h2>
              <small>Based on execution timeline</small>
            </div>
          </div>

          <div className="cpu-panel gantt-panel">
            <div className="cpu-panel-heading">
              <h3>Gantt Chart</h3>
              <p>Process execution timeline</p>
            </div>

            {ganttChart.length > 0 ? (
              <>
                <div className="gantt-chart">
                  {ganttChart.map((segment, index) => {
                    const duration =
                      segment.endTime - segment.startTime;

                    return (
                      <div
                        key={`${segment.pid}-${segment.startTime}-${index}`}
                        className={`gantt-block ${getGanttClass(
                          segment.pid
                        )}`}
                        style={{
                          flexGrow: Math.max(duration, 0.1),
                          flexBasis: 0,
                        }}
                        title={`${
                          segment.pid > 0
                            ? `P${segment.pid}`
                            : "Idle"
                        }: ${segment.startTime} to ${segment.endTime}`}
                      >
                        <strong>
                          {segment.pid > 0
                            ? `P${segment.pid}`
                            : "Idle"}
                        </strong>
                        <span>
                          {segment.startTime} – {segment.endTime}
                        </span>
                      </div>
                    );
                  })}
                </div>

                <div className="gantt-timeline">
                  {ganttChart.map((segment, index) => (
                    <span key={`time-${index}`}>
                      {segment.startTime}
                    </span>
                  ))}
                  <span>{ganttChart[ganttChart.length - 1].endTime}</span>
                </div>
              </>
            ) : (
              <p>No execution segments were returned.</p>
            )}
          </div>

          <div className="cpu-panel">
            <div className="cpu-panel-heading">
              <h3>Process Metrics</h3>
              <p>Execution details for each process</p>
            </div>

            <div className="table-scroll">
              <table className="process-table">
                <thead>
                  <tr>
                    <th>Process</th>
                    <th>Completion Time</th>
                    <th>Turnaround Time</th>
                    <th>Waiting Time</th>
                    <th>Response Time</th>
                  </tr>
                </thead>

                <tbody>
                  {result.metrics.map((metric) => (
                    <tr key={metric.pid}>
                      <td>
                        <span className="pid-badge">P{metric.pid}</span>
                      </td>
                      <td>{metric.completionTime}</td>
                      <td>{metric.turnaroundTime}</td>
                      <td>{metric.waitingTime}</td>
                      <td>{metric.responseTime}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>

          <div className="cpu-panel">
            <div className="cpu-panel-heading">
              <h3>Scheduling Decision Log</h3>
              <p>Why the scheduler selected each process</p>
            </div>

            {result.decisions.length > 0 ? (
              <div className="decision-list">
                {result.decisions.map((decision, index) => (
                  <div
                    className="decision-item"
                    key={`${decision.time}-${index}`}
                  >
                    <span className="decision-time">
                      Time {decision.time}
                    </span>
                    <strong>
                      {decision.selectedPid > 0
                        ? `P${decision.selectedPid}`
                        : "Idle"}
                    </strong>
                    <p>{decision.reason}</p>
                  </div>
                ))}
              </div>
            ) : (
              <p>No scheduling decisions were returned.</p>
            )}
          </div>
        </section>
      )}
    </div>
  );
}

export default CPUScheduling;