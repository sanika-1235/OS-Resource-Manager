
import { useEffect, useMemo, useState } from "react";
import "./ProcessManagement.css";

const API_URL = "http://localhost:8080/api/processes";

const stateOptions = ["NEW", "READY", "RUNNING", "WAITING"];

const allowedTransitions = {
  NEW: ["READY"],
  READY: ["RUNNING"],
  RUNNING: ["READY", "WAITING"],
  WAITING: ["READY"],
  TERMINATED: [],
};

function ProcessManagement() {
  const [processes, setProcesses] = useState([]);
  const [form, setForm] = useState({
    name: "",
    arrivalTime: "",
    burstTime: "",
    priority: "1",
  });
  const [search, setSearch] = useState("");
  const [error, setError] = useState("");
  const [loading, setLoading] = useState(true);
  const [busy, setBusy] = useState(false);

  // Load processes from the C++ backend.
  const loadProcesses = async () => {
    try {
      setError("");

      const response = await fetch(API_URL);

      if (!response.ok) {
        throw new Error("Could not load processes from the backend.");
      }

      const data = await response.json();
      setProcesses(data);
    } catch (err) {
      setError(err.message || "Unable to connect to the backend.");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadProcesses();
  }, []);

  const counts = {
    total: processes.length,
    running: processes.filter((p) => p.state === "RUNNING").length,
    ready: processes.filter((p) => p.state === "READY").length,
    waiting: processes.filter((p) => p.state === "WAITING").length,
    new: processes.filter((p) => p.state === "NEW").length,
    terminated: processes.filter((p) => p.state === "TERMINATED").length,
  };

  const filteredProcesses = useMemo(
    () =>
      processes.filter((process) =>
        `${process.pid} ${process.name}`
          .toLowerCase()
          .includes(search.toLowerCase())
      ),
    [processes, search]
  );

  // Create a process through POST.
  const handleAddProcess = async (event) => {
    event.preventDefault();
    setError("");

    const arrival = Number(form.arrivalTime);
    const burst = Number(form.burstTime);
    const priority = Number(form.priority);

    if (!form.name.trim()) {
      setError("Please enter a process name.");
      return;
    }

    if (
      form.arrivalTime === "" ||
      form.burstTime === "" ||
      arrival < 0 ||
      burst <= 0 ||
      form.priority === "" ||
      priority < 0
    ) {
      setError(
        "Arrival time must be 0 or more. Burst time must be positive and priority must be 0 or more."
      );
      return;
    }

    try {
      setBusy(true);

      const response = await fetch(API_URL, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          name: form.name.trim(),
          arrivalTime: arrival,
          burstTime: burst,
          priority,
        }),
      });

      const result = await response.json();

      if (!response.ok) {
        throw new Error(result.error || "Failed to create process.");
      }

      setForm({
        name: "",
        arrivalTime: "",
        burstTime: "",
        priority: "1",
      });

      await loadProcesses();
    } catch (err) {
      setError(err.message || "Failed to create process.");
    } finally {
      setBusy(false);
    }
  };

  // Update process state through PUT.
  const updateProcessState = async (pid, newState) => {
    setError("");

    try {
      setBusy(true);

      const response = await fetch(`${API_URL}/${pid}/state`, {
        method: "PUT",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ state: newState }),
      });

      const result = await response.json();

      if (!response.ok) {
        throw new Error(result.error || "Invalid process state transition.");
      }

      await loadProcesses();
    } catch (err) {
      setError(err.message || "Could not update process state.");
    } finally {
      setBusy(false);
    }
  };

  // Terminate process through DELETE.
  const terminateProcess = async (pid) => {
    setError("");

    try {
      setBusy(true);

      const response = await fetch(`${API_URL}/${pid}`, {
        method: "DELETE",
      });

      const result = await response.json();

      if (!response.ok) {
        throw new Error(result.error || "Could not terminate process.");
      }

      await loadProcesses();
    } catch (err) {
      setError(err.message || "Could not terminate process.");
    } finally {
      setBusy(false);
    }
  };

  // Refresh data from backend.
  const resetProcesses = async () => {
    setSearch("");
    await loadProcesses();
  };

  return (
    <div className="process-page">
      <div className="process-page-heading">
        <div>
          <h2>Process Management</h2>
          <p>Create, monitor and manage system processes.</p>
        </div>

        <button
          className="process-reset-btn"
          onClick={resetProcesses}
          disabled={busy}
        >
          Refresh Processes
        </button>
      </div>

      <div className="process-stats">
        <div className="process-stat-card">
          <span>Total Processes</span>
          <strong>{counts.total}</strong>
          <small>All created processes</small>
        </div>

        <div className="process-stat-card">
          <span>Running</span>
          <strong className="stat-running">{counts.running}</strong>
          <small>Currently executing</small>
        </div>

        <div className="process-stat-card">
          <span>Ready</span>
          <strong className="stat-ready">{counts.ready}</strong>
          <small>Waiting for CPU</small>
        </div>

        <div className="process-stat-card">
          <span>Waiting</span>
          <strong className="stat-waiting">{counts.waiting}</strong>
          <small>Waiting for an event</small>
        </div>

        <div className="process-stat-card">
          <span>Terminated</span>
          <strong className="stat-terminated">{counts.terminated}</strong>
          <small>Completed or stopped</small>
        </div>
      </div>

      <div className="process-content-grid">
        <section className="process-panel process-form-panel">
          <div className="process-panel-heading">
            <div>
              <h3>Create Process</h3>
              <p>Add a new process to the simulation.</p>
            </div>
            <span className="process-heading-icon">＋</span>
          </div>

          <form onSubmit={handleAddProcess} className="process-form">
            <label>
              Process Name
              <input
                type="text"
                placeholder="e.g. Chrome"
                value={form.name}
                onChange={(event) =>
                  setForm({ ...form, name: event.target.value })
                }
              />
            </label>

            <div className="process-form-row">
              <label>
                Arrival Time
                <input
                  type="number"
                  min="0"
                  placeholder="0"
                  value={form.arrivalTime}
                  onChange={(event) =>
                    setForm({ ...form, arrivalTime: event.target.value })
                  }
                />
              </label>

              <label>
                Burst Time
                <input
                  type="number"
                  min="1"
                  placeholder="5"
                  value={form.burstTime}
                  onChange={(event) =>
                    setForm({ ...form, burstTime: event.target.value })
                  }
                />
              </label>
            </div>

            <label>
              Priority
              <input
                type="number"
                min="0"
                placeholder="1"
                value={form.priority}
                onChange={(event) =>
                  setForm({ ...form, priority: event.target.value })
                }
              />
            </label>

            {error && <p className="process-error">{error}</p>}

            <button
              type="submit"
              className="process-add-btn"
              disabled={busy}
            >
              {busy ? "Please wait..." : "+ Add Process"}
            </button>
          </form>
        </section>

        <section className="process-panel process-state-panel">
          <div className="process-panel-heading">
            <div>
              <h3>Process States</h3>
              <p>Current distribution of processes.</p>
            </div>
          </div>

          {stateOptions.map((state) => {
            const count = processes.filter(
              (process) => process.state === state
            ).length;

            return (
              <div className="process-state-row" key={state}>
                <div className="process-state-label">
                  <span className={`state-dot ${state.toLowerCase()}`}></span>
                  <span>{state}</span>
                </div>

                <strong>{count}</strong>

                <div className="process-state-track">
                  <div
                    className={`process-state-fill ${state.toLowerCase()}`}
                    style={{
                      width: `${
                        processes.length
                          ? (count / processes.length) * 100
                          : 0
                      }%`,
                    }}
                  />
                </div>
              </div>
            );
          })}

          <div className="process-state-note">
            <strong>Process lifecycle</strong>
            <p>NEW → READY → RUNNING → WAITING / TERMINATED</p>
            <small>State changes are validated by the C++ backend.</small>
          </div>
        </section>
      </div>

      <section className="process-panel process-table-panel">
        <div className="process-table-heading">
          <div>
            <h3>Process Table</h3>
            <p>View and manage all processes in the system.</p>
          </div>

          <input
            className="process-search"
            type="text"
            placeholder="Search process..."
            value={search}
            onChange={(event) => setSearch(event.target.value)}
          />
        </div>

        {loading ? (
          <p>Loading processes...</p>
        ) : (
          <div className="process-table-wrapper">
            <table className="process-table">
              <thead>
                <tr>
                  <th>PID</th>
                  <th>Process Name</th>
                  <th>Arrival</th>
                  <th>Burst</th>
                  <th>Remaining</th>
                  <th>Priority</th>
                  <th>State</th>
                  <th>Actions</th>
                </tr>
              </thead>

              <tbody>
                {filteredProcesses.length > 0 ? (
                  filteredProcesses.map((process) => (
                    <tr key={process.pid}>
                      <td className="process-pid">P{process.pid}</td>
                      <td>{process.name}</td>
                      <td>{process.arrivalTime}</td>
                      <td>{process.burstTime}</td>
                      <td>{process.remainingTime}</td>
                      <td>{process.priority}</td>

                      <td>
                        <span
                          className={`process-state-badge ${process.state.toLowerCase()}`}
                        >
                          {process.state}
                        </span>
                      </td>

                      <td>
                        <div className="process-actions">
                          {process.state !== "TERMINATED" ? (
                            <>
                              <select
                                value={process.state}
                                disabled={busy}
                                onChange={(event) =>
                                  updateProcessState(
                                    process.pid,
                                    event.target.value
                                  )
                                }
                                aria-label={`Change state for ${process.name}`}
                              >
                                <option value={process.state}>
                                  {process.state}
                                </option>

                                {allowedTransitions[process.state].map(
                                  (state) => (
                                    <option key={state} value={state}>
                                      {state}
                                    </option>
                                  )
                                )}
                              </select>

                              <button
                                className="process-terminate-btn"
                                disabled={busy}
                                onClick={() => terminateProcess(process.pid)}
                              >
                                Terminate
                              </button>
                            </>
                          ) : (
                            <span className="process-finished">Finished</span>
                          )}
                        </div>
                      </td>
                    </tr>
                  ))
                ) : (
                  <tr>
                    <td colSpan="8" className="process-empty">
                      No processes found.
                    </td>
                  </tr>
                )}
              </tbody>
            </table>
          </div>
        )}

        <div className="process-table-footer">
          Showing {filteredProcesses.length} of {processes.length} processes
        </div>
      </section>

      <p className="process-demo-note">
        Process data is connected to the C++ backend through HTTP APIs.
      </p>
    </div>
  );
}

export default ProcessManagement;
