
import { useState } from "react";
import "./DeadlockManagement.css";

const initialData = {
  available: [3, 3, 2],
  processes: [
    { id: "P0", allocation: [0, 1, 0], maximum: [7, 5, 3] },
    { id: "P1", allocation: [2, 0, 0], maximum: [3, 2, 2] },
    { id: "P2", allocation: [3, 0, 2], maximum: [9, 0, 2] },
    { id: "P3", allocation: [2, 1, 1], maximum: [2, 2, 2] },
    { id: "P4", allocation: [0, 0, 2], maximum: [4, 3, 3] },
  ],
};

const resourceNames = ["R1", "R2", "R3"];

function DeadlockManagement() {
  const [data, setData] = useState(initialData);
  const [result, setResult] = useState(null);
  const [requestProcess, setRequestProcess] = useState("P1");
  const [request, setRequest] = useState(["1", "0", "2"]);
  const [message, setMessage] = useState(
    "Run the safety check to analyze the current resource state."
  );

  const calculateNeed = () =>
    data.processes.map((process) =>
      process.maximum.map(
        (maximum, index) => maximum - process.allocation[index]
      )
    );

  const detectDeadlock = () => {
    const need = calculateNeed();
    const work = [...data.available];
    const finished = data.processes.map(() => false);
    const safeSequence = [];

    let progress = true;

    while (progress) {
      progress = false;

      for (let i = 0; i < data.processes.length; i++) {
        if (finished[i]) continue;

        const canFinish = need[i].every(
          (required, j) => required <= work[j]
        );

        if (canFinish) {
          work.forEach((_, j) => {
            work[j] += data.processes[i].allocation[j];
          });

          finished[i] = true;
          safeSequence.push(data.processes[i].id);
          progress = true;
        }
      }
    }

    const safe = finished.every(Boolean);
    setResult({ safe, safeSequence });
    setMessage(
      safe
        ? "All processes can finish in a safe order."
        : "The system is in an unsafe state. A safe sequence was not found."
    );
  };

  const requestResources = () => {
    const processIndex = data.processes.findIndex(
      (process) => process.id === requestProcess
    );
    const requested = request.map(Number);

    if (
      requested.some((value) => !Number.isInteger(value) || value < 0) ||
      requested.every((value) => value === 0)
    ) {
      setMessage("Enter valid positive resource quantities.");
      return;
    }

    const process = data.processes[processIndex];
    const need = process.maximum.map(
      (maximum, index) => maximum - process.allocation[index]
    );

    if (requested.some((value, index) => value > need[index])) {
      setMessage("Request exceeds the process's declared maximum need.");
      return;
    }

    if (requested.some((value, index) => value > data.available[index])) {
      setMessage("Request cannot be granted: insufficient available resources.");
      return;
    }

    const updated = {
      available: data.available.map(
        (value, index) => value - requested[index]
      ),
      processes: data.processes.map((item, index) =>
        index === processIndex
          ? {
              ...item,
              allocation: item.allocation.map(
                (value, j) => value + requested[j]
              ),
            }
          : item
      ),
    };

    setData(updated);
    setResult(null);
    setMessage(
      `Request granted to ${requestProcess}. Run the safety check to verify the new state.`
    );
  };

  const resetSimulation = () => {
    setData(initialData);
    setResult(null);
    setRequestProcess("P1");
    setRequest(["1", "0", "2"]);
    setMessage("Simulation reset to the initial resource state.");
  };

  const need = calculateNeed();

  return (
    <div className="deadlock-page">
      <div className="deadlock-intro">
        <div>
          <h2>Deadlock Management</h2>
          <p>
            Monitor resource allocation, handle requests and detect unsafe
            system states.
          </p>
        </div>
        <span className="deadlock-tag">DEADLOCK MODULE</span>
      </div>

      <div className="deadlock-stats">
        <div className="deadlock-stat">
          <span>Processes</span>
          <h2>{data.processes.length}</h2>
          <small>Active processes</small>
        </div>
        <div className="deadlock-stat">
          <span>Resource Types</span>
          <h2>{resourceNames.length}</h2>
          <small>Resource categories</small>
        </div>
        <div className="deadlock-stat">
          <span>Available Resources</span>
          <h2>{data.available.reduce((a, b) => a + b, 0)}</h2>
          <small>Total available instances</small>
        </div>
        <div className="deadlock-stat">
          <span>System Status</span>
          <h2 className={result ? (result.safe ? "safe-text" : "unsafe-text") : ""}>
            {result ? (result.safe ? "Safe" : "Unsafe") : "Not checked"}
          </h2>
          <small>Safety analysis</small>
        </div>
      </div>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Available Resources</h3>
            <p>Instances currently available for allocation.</p>
          </div>
        </div>

        <div className="resource-cards">
          {resourceNames.map((name, index) => (
            <div className="resource-card" key={name}>
              <span>{name}</span>
              <strong>{data.available[index]}</strong>
              <small>Available instances</small>
            </div>
          ))}
        </div>
      </section>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Resource Allocation Table</h3>
            <p>Allocation, maximum requirement and remaining need per process.</p>
          </div>
        </div>

        <div className="deadlock-table-scroll">
          <table className="deadlock-table">
            <thead>
              <tr>
                <th rowSpan="2">Process</th>
                <th colSpan="3">Allocation</th>
                <th colSpan="3">Maximum</th>
                <th colSpan="3">Need</th>
              </tr>
              <tr>
                {resourceNames.map((name) => <th key={`a${name}`}>{name}</th>)}
                {resourceNames.map((name) => <th key={`m${name}`}>{name}</th>)}
                {resourceNames.map((name) => <th key={`n${name}`}>{name}</th>)}
              </tr>
            </thead>
            <tbody>
              {data.processes.map((process, index) => (
                <tr key={process.id}>
                  <td><span className="deadlock-pid">{process.id}</span></td>
                  {process.allocation.map((value, j) => <td key={`a${j}`}>{value}</td>)}
                  {process.maximum.map((value, j) => <td key={`m${j}`}>{value}</td>)}
                  {need[index].map((value, j) => <td key={`n${j}`}>{value}</td>)}
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </section>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Resource Request</h3>
            <p>Request additional resource instances for a process.</p>
          </div>
        </div>

        <div className="request-controls">
          <div className="deadlock-field">
            <label>Process</label>
            <select
              value={requestProcess}
              onChange={(event) => setRequestProcess(event.target.value)}
            >
              {data.processes.map((process) => (
                <option key={process.id}>{process.id}</option>
              ))}
            </select>
          </div>

          {resourceNames.map((name, index) => (
            <div className="deadlock-field" key={name}>
              <label>{name} Request</label>
              <input
                type="number"
                min="0"
                value={request[index]}
                onChange={(event) =>
                  setRequest((old) =>
                    old.map((value, i) =>
                      i === index ? event.target.value : value
                    )
                  )
                }
              />
            </div>
          ))}

          <button className="request-button" onClick={requestResources}>
            Submit Request
          </button>
        </div>
      </section>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Resource Allocation Graph</h3>
            <p>Schematic view of processes and resource ownership.</p>
          </div>
        </div>

        <div className="allocation-graph">
          {data.processes.map((process) => (
            <div className="graph-process" key={process.id}>
              <strong>{process.id}</strong>
              <div className="graph-resources">
                {process.allocation.map((count, index) =>
                  count > 0 ? (
                    <span key={index}>
                      {resourceNames[index]} × {count}
                    </span>
                  ) : null
                )}
                {!process.allocation.some((count) => count > 0) && (
                  <span className="no-resource">No allocated resources</span>
                )}
              </div>
            </div>
          ))}
        </div>
        <p className="graph-caption">
          This is a simplified ownership view, not a full directed resource
          allocation graph.
        </p>
      </section>

      <section className="deadlock-panel detection-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Deadlock Detection</h3>
            <p>Run the Banker's safety algorithm on the current resource state.</p>
          </div>
        </div>

        <div className="detection-actions">
          <button className="detect-button" onClick={detectDeadlock}>
            Run Safety Check
          </button>
          <button className="deadlock-reset" onClick={resetSimulation}>
            Reset Simulation
          </button>
        </div>

        <div className="deadlock-message">{message}</div>

        {result && (
          <div className={`safety-result ${result.safe ? "safe-result" : "unsafe-result"}`}>
            <strong>
              {result.safe ? "System is in a SAFE state" : "System is in an UNSAFE state"}
            </strong>
            {result.safe ? (
              <div className="safe-sequence">
                <span>Safe sequence</span>
                <div>
                  {result.safeSequence.map((process, index) => (
                    <span className="sequence-process" key={process}>
                      {process}
                      {index < result.safeSequence.length - 1 && (
                        <span className="sequence-arrow">→</span>
                      )}
                    </span>
                  ))}
                </div>
              </div>
            ) : (
              <p>No safe sequence could be found for the current state.</p>
            )}
          </div>
        )}
      </section>
    </div>
  );
}

export default DeadlockManagement;
