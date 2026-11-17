
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

function calculateNeed(data) {
  return data.processes.map((process) =>
    process.maximum.map(
      (maximum, index) => maximum - process.allocation[index]
    )
  );
}

function validateData(data) {
  if (data.processes.length === 0) {
    return "Add at least one process before running the safety check.";
  }

  if (data.available.some((value) => !Number.isInteger(value) || value < 0)) {
    return "Available resource values must be non-negative integers.";
  }

  for (const process of data.processes) {
    for (let j = 0; j < resourceNames.length; j++) {
      const allocated = process.allocation[j];
      const maximum = process.maximum[j];

      if (
        !Number.isInteger(allocated) ||
        !Number.isInteger(maximum) ||
        allocated < 0 ||
        maximum < 0
      ) {
        return `${process.id}: Allocation and Maximum must be non-negative integers.`;
      }

      if (allocated > maximum) {
        return `${process.id}: Allocation of ${resourceNames[j]} cannot exceed its Maximum.`;
      }
    }
  }

  return null;
}

function runSafetyAnalysis(data) {
  const validationError = validateData(data);

  if (validationError) {
    return { valid: false, error: validationError };
  }

  const need = calculateNeed(data);
  const work = [...data.available];
  const finished = data.processes.map(() => false);
  const safeSequence = [];
  const steps = [];

  while (safeSequence.length < data.processes.length) {
    let progress = false;

    for (let i = 0; i < data.processes.length; i++) {
      if (finished[i]) continue;

      const canFinish = need[i].every(
        (required, j) => required <= work[j]
      );

      if (canFinish) {
        const workBefore = [...work];

        work.forEach((_, j) => {
          work[j] += data.processes[i].allocation[j];
        });

        finished[i] = true;
        safeSequence.push(data.processes[i].id);

        steps.push({
          process: data.processes[i].id,
          need: [...need[i]],
          workBefore,
          workAfter: [...work],
        });

        progress = true;
      }
    }

    if (!progress) break;
  }

  const blocked = data.processes
    .map((process, index) => ({
      id: process.id,
      need: need[index],
      work: [...work],
      shortages: need[index].map((required, j) =>
        Math.max(0, required - work[j])
      ),
      finished: finished[index],
    }))
    .filter((item) => !item.finished);

  return {
    valid: true,
    safe: safeSequence.length === data.processes.length,
    safeSequence,
    steps,
    blocked,
    finalWork: work,
    need,
  };
}

function DeadlockManagement() {
  const [data, setData] = useState(initialData);
  const [result, setResult] = useState(null);
  const [requestProcess, setRequestProcess] = useState("P1");
  const [request, setRequest] = useState(["1", "0", "2"]);
  const [nextProcessNumber, setNextProcessNumber] = useState(5);
  const [message, setMessage] = useState(
    "Edit the resource values, then run the safety check."
  );

  const need = calculateNeed(data);
  const validationError = validateData(data);

  const updateAvailable = (index, rawValue) => {
    const value = rawValue === "" ? 0 : Number(rawValue);

    setData((old) => ({
      ...old,
      available: old.available.map((item, i) =>
        i === index ? value : item
      ),
    }));

    setResult(null);
    setMessage("Resource values changed. Run the safety check again.");
  };

  const updateProcessValue = (
    processIndex,
    field,
    resourceIndex,
    rawValue
  ) => {
    const value = rawValue === "" ? 0 : Number(rawValue);

    setData((old) => ({
      ...old,
      processes: old.processes.map((process, index) =>
        index === processIndex
          ? {
              ...process,
              [field]: process[field].map((item, j) =>
                j === resourceIndex ? value : item
              ),
            }
          : process
      ),
    }));

    setResult(null);
    setMessage("Resource values changed. Need will update automatically.");
  };

  // ADD A NEW PROCESS
  const addProcess = () => {
    const newId = `P${nextProcessNumber}`;

    const newProcess = {
      id: newId,
      allocation: [0, 0, 0],
      maximum: [1, 1, 1],
    };

    setData((old) => ({
      ...old,
      processes: [...old.processes, newProcess],
    }));

    setNextProcessNumber((old) => old + 1);
    setRequestProcess(newId);
    setResult(null);
    setMessage(
      `${newId} added successfully. Edit its Maximum and Allocation values as required.`
    );
  };

  // DELETE A PROCESS
  const deleteProcess = (processId) => {
    if (data.processes.length <= 1) {
      setMessage("At least one process must remain in the system.");
      return;
    }

    const updatedProcesses = data.processes.filter(
      (process) => process.id !== processId
    );

    setData((old) => ({
      ...old,
      processes: updatedProcesses,
    }));

    if (requestProcess === processId) {
      setRequestProcess(updatedProcesses[0].id);
    }

    setResult(null);
    setMessage(`${processId} deleted successfully.`);
  };

  const detectDeadlock = () => {
    const analysis = runSafetyAnalysis(data);

    if (!analysis.valid) {
      setResult(null);
      setMessage(analysis.error);
      return;
    }

    setResult(analysis);
    setMessage(
      analysis.safe
        ? "Every process can complete. The system has at least one safe sequence."
        : "Not all processes can complete with the current available resources."
    );
  };

  const requestResources = () => {
    const currentValidationError = validateData(data);

    if (currentValidationError) {
      setMessage(currentValidationError);
      return;
    }

    const processIndex = data.processes.findIndex(
      (process) => process.id === requestProcess
    );

    if (processIndex === -1) {
      setMessage("Please select a valid process.");
      return;
    }

    const requested = request.map(Number);

    if (
      requested.some(
        (value) => !Number.isInteger(value) || value < 0
      ) ||
      requested.every((value) => value === 0)
    ) {
      setMessage(
        "Enter non-negative whole numbers, with at least one positive request."
      );
      return;
    }

    const process = data.processes[processIndex];

    const processNeed = process.maximum.map(
      (maximum, index) => maximum - process.allocation[index]
    );

    if (requested.some((value, index) => value > processNeed[index])) {
      setMessage("Request denied: it exceeds the process's remaining Need.");
      return;
    }

    if (requested.some((value, index) => value > data.available[index])) {
      setMessage("Request denied: there are not enough Available resources.");
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

    const analysis = runSafetyAnalysis(updated);

    if (!analysis.safe) {
      setResult(analysis);
      setMessage(
        `Request denied: granting these resources to ${requestProcess} would leave the system in an unsafe state.`
      );
      return;
    }

    setData(updated);
    setResult(analysis);
    setMessage(
      `Request granted to ${requestProcess}. The resulting state is safe.`
    );
  };

  const resetSimulation = () => {
    setData({
      available: [...initialData.available],
      processes: initialData.processes.map((process) => ({
        ...process,
        allocation: [...process.allocation],
        maximum: [...process.maximum],
      })),
    });

    setResult(null);
    setRequestProcess("P1");
    setRequest(["1", "0", "2"]);
    setNextProcessNumber(5);
    setMessage("Simulation reset to the original Banker's Algorithm example.");
  };

  return (
    <div className="deadlock-page">
      <div className="deadlock-intro">
        <div>
          <h2>Deadlock Management</h2>
          <p>
            Experiment with resource allocation, process requirements and
            Banker's safety algorithm.
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
          <h2>
            {data.available.reduce(
              (total, value) =>
                total + (Number.isFinite(value) ? value : 0),
              0
            )}
          </h2>
          <small>Total available instances</small>
        </div>

        <div className="deadlock-stat">
          <span>System Status</span>
          <h2
            className={
              result
                ? result.safe
                  ? "safe-text"
                  : "unsafe-text"
                : ""
            }
          >
            {validationError
              ? "Invalid input"
              : result
                ? result.safe
                  ? "Safe"
                  : "Unsafe"
                : "Not checked"}
          </h2>
          <small>Safety analysis</small>
        </div>
      </div>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Available Resources</h3>
            <p>
              Edit the number of resource instances currently available.
            </p>
          </div>
          <span className="edit-hint">Editable values</span>
        </div>

        <div className="resource-cards">
          {resourceNames.map((name, index) => (
            <div className="resource-card" key={name}>
              <span>{name}</span>
              <input
                className="resource-edit-input"
                type="number"
                min="0"
                step="1"
                value={data.available[index]}
                aria-label={`Available ${name}`}
                onChange={(event) =>
                  updateAvailable(index, event.target.value)
                }
              />
              <small>Available instances</small>
            </div>
          ))}
        </div>
      </section>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Resource Allocation Table</h3>
            <p>
              Edit Allocation and Maximum. Need is calculated automatically
              using Maximum − Allocation.
            </p>
          </div>

          <div className="process-actions">
            <span className="edit-hint">Allocation &amp; Maximum editable</span>
            <button
              type="button"
              className="add-process-button"
              onClick={addProcess}
            >
              + Add Process
            </button>
          </div>
        </div>

        <div className="deadlock-table-scroll">
          <table className="deadlock-table editable-deadlock-table">
            <thead>
              <tr>
                <th rowSpan="2">Process</th>
                <th colSpan="3">Allocation</th>
                <th colSpan="3">Maximum</th>
                <th colSpan="3">Need (calculated)</th>
                <th rowSpan="2">Action</th>
              </tr>
              <tr>
                {resourceNames.map((name) => (
                  <th key={`a${name}`}>{name}</th>
                ))}
                {resourceNames.map((name) => (
                  <th key={`m${name}`}>{name}</th>
                ))}
                {resourceNames.map((name) => (
                  <th key={`n${name}`}>{name}</th>
                ))}
              </tr>
            </thead>

            <tbody>
              {data.processes.map((process, index) => (
                <tr key={process.id}>
                  <td>
                    <span className="deadlock-pid">{process.id}</span>
                  </td>

                  {process.allocation.map((value, j) => (
                    <td key={`a${j}`}>
                      <input
                        className="matrix-input"
                        type="number"
                        min="0"
                        step="1"
                        value={value}
                        aria-label={`${process.id} Allocation ${resourceNames[j]}`}
                        onChange={(event) =>
                          updateProcessValue(
                            index,
                            "allocation",
                            j,
                            event.target.value
                          )
                        }
                      />
                    </td>
                  ))}

                  {process.maximum.map((value, j) => (
                    <td key={`m${j}`}>
                      <input
                        className="matrix-input maximum-input"
                        type="number"
                        min="0"
                        step="1"
                        value={value}
                        aria-label={`${process.id} Maximum ${resourceNames[j]}`}
                        onChange={(event) =>
                          updateProcessValue(
                            index,
                            "maximum",
                            j,
                            event.target.value
                          )
                        }
                      />
                    </td>
                  ))}

                  {need[index].map((value, j) => (
                    <td className="need-cell" key={`n${j}`}>
                      {value}
                    </td>
                  ))}

                  <td>
                    <button
                      type="button"
                      className="delete-process-button"
                      onClick={() => deleteProcess(process.id)}
                      disabled={data.processes.length === 1}
                      title={
                        data.processes.length === 1
                          ? "At least one process must remain"
                          : `Delete ${process.id}`
                      }
                    >
                      Delete
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        {validationError && (
          <div className="deadlock-validation">
            <strong>Input validation:</strong> {validationError}
          </div>
        )}

        <p className="table-caption">
          Tip: Add or delete processes dynamically. Change Allocation,
          Maximum or Available values and watch the Need matrix update
          immediately. Values must be whole numbers, and Allocation cannot
          exceed Maximum.
        </p>
      </section>

      <section className="deadlock-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Resource Request</h3>
            <p>
              Request additional resource instances. The request is granted
              only if the resulting state remains safe.
            </p>
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
                <option key={process.id} value={process.id}>
                  {process.id}
                </option>
              ))}
            </select>
          </div>

          {resourceNames.map((name, index) => (
            <div className="deadlock-field" key={name}>
              <label>{name} Request</label>
              <input
                type="number"
                min="0"
                step="1"
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

          <button
            className="request-button"
            onClick={requestResources}
            disabled={Boolean(validationError)}
          >
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
                  <span className="no-resource">
                    No allocated resources
                  </span>
                )}
              </div>
            </div>
          ))}
        </div>

        <p className="graph-caption">
          This is a simplified ownership view, not a full directed resource
          allocation graph. It updates when Allocation values change.
        </p>
      </section>

      <section className="deadlock-panel detection-panel">
        <div className="deadlock-panel-heading">
          <div>
            <h3>Deadlock Detection</h3>
            <p>
              Run the Banker's safety algorithm on your current resource
              configuration.
            </p>
          </div>
        </div>

        <div className="detection-actions">
          <button
            className="detect-button"
            onClick={detectDeadlock}
            disabled={Boolean(validationError)}
          >
            Run Safety Check
          </button>

          <button className="deadlock-reset" onClick={resetSimulation}>
            Reset Simulation
          </button>
        </div>

        <div className="deadlock-message">
          {validationError || message}
        </div>

        {result && result.valid && (
          <div
            className={`safety-result ${
              result.safe ? "safe-result" : "unsafe-result"
            }`}
          >
            <strong>
              {result.safe
                ? "System is in a SAFE state"
                : "System is in an UNSAFE state"}
            </strong>

            {result.safe ? (
              <>
                <p>
                  All {data.processes.length} processes can finish in the
                  following order without running out of resources.
                </p>

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
              </>
            ) : (
              <>
                <p>
                  The algorithm could not find a process that can finish
                  using the remaining Work resources. This is an unsafe
                  state; it does not necessarily mean deadlock has already
                  occurred.
                </p>

                <div className="blocked-processes">
                  <strong>Processes that could not finish:</strong>
                  {result.blocked.map((item) => (
                    <div className="blocked-process" key={item.id}>
                      <b>{item.id}</b>
                      <span>Need: [{item.need.join(", ")}]</span>
                      <span>Work: [{item.work.join(", ")}]</span>
                      <span>
                        Shortage: [{item.shortages.join(", ")}]
                      </span>
                    </div>
                  ))}
                </div>
              </>
            )}

            <div className="analysis-steps">
              <h4>Safety Algorithm Walkthrough</h4>

              {result.steps.map((step, index) => (
                <div className="analysis-step" key={step.process}>
                  <div className="analysis-step-heading">
                    <span className="step-number">{index + 1}</span>
                    <strong>{step.process} can finish</strong>
                  </div>

                  <p>
                    Need [{step.need.join(", ")}] is less than or equal to
                    Work [{step.workBefore.join(", ")}].
                  </p>

                  <p>
                    After {step.process} finishes, it releases its allocated
                    resources. New Work = [{step.workAfter.join(", ")}].
                  </p>
                </div>
              ))}

              {result.safe && (
                <p className="walkthrough-complete">
                  All processes finished. The system is safe.
                </p>
              )}

              {!result.safe && (
                <p className="walkthrough-incomplete">
                  No remaining process can satisfy Need ≤ Work. The
                  algorithm stops here.
                </p>
              )}
            </div>
          </div>
        )}
      </section>
    </div>
  );
}

export default DeadlockManagement;
