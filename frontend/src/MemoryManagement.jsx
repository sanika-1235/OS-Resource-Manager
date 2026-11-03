
import { useMemo, useState } from "react";
import "./MemoryManagement.css";

const initialBlocks = [
  { id: 1, size: 100, process: null },
  { id: 2, size: 200, process: null },
  { id: 3, size: 300, process: null },
  { id: 4, size: 400, process: null },
];

function MemoryManagement() {
  const [blocks, setBlocks] = useState(initialBlocks);
  const [processName, setProcessName] = useState("P1");
  const [processSize, setProcessSize] = useState("120");
  const [algorithm, setAlgorithm] = useState("First Fit");
  const [message, setMessage] = useState("Choose a process size and allocate memory.");

  const allocated = blocks.filter((block) => block.process !== null);
  const usedMemory = allocated.reduce((sum, block) => sum + block.size, 0);
  const totalMemory = blocks.reduce((sum, block) => sum + block.size, 0);
  const freeMemory = totalMemory - usedMemory;
  const utilization = totalMemory ? (usedMemory / totalMemory) * 100 : 0;
  const internalFragmentation = allocated.reduce(
    (sum, block) => sum + block.size - block.process.size,
    0
  );

  const nextProcessNumber = useMemo(() => {
    const numbers = blocks
      .filter((block) => block.process)
      .map((block) => Number(block.process.name.replace(/\D/g, "")) || 0);
    return numbers.length ? Math.max(...numbers) + 1 : 1;
  }, [blocks]);

  const allocateMemory = () => {
    const size = Number(processSize);
    const name = processName.trim();

    if (!name || !Number.isInteger(size) || size <= 0) {
      setMessage("Enter a valid process name and a positive memory size.");
      return;
    }

    if (blocks.some((block) => block.process?.name === name)) {
      setMessage(`${name} is already allocated.`);
      return;
    }

    const candidates = blocks
      .map((block, index) => ({ ...block, index }))
      .filter((block) => !block.process && block.size >= size);

    if (candidates.length === 0) {
      setMessage(`Allocation failed: no free block can fit ${size} units.`);
      return;
    }

    let selected;
    if (algorithm === "Best Fit") {
      selected = candidates.reduce((best, item) =>
        item.size < best.size ? item : best
      );
    } else if (algorithm === "Worst Fit") {
      selected = candidates.reduce((best, item) =>
        item.size > best.size ? item : best
      );
    } else {
      selected = candidates[0];
    }

    const updated = [...blocks];
    updated[selected.index] = {
      ...updated[selected.index],
      process: { name, size },
    };

    setBlocks(updated);
    setMessage(
      `${name} allocated successfully in Block ${selected.id} using ${algorithm}.`
    );
    setProcessName(`P${nextProcessNumber + 1}`);
  };

  const deallocateMemory = (blockId) => {
    const updated = blocks.map((block) =>
      block.id === blockId ? { ...block, process: null } : block
    );
    setBlocks(updated);
    setMessage(`Memory released from Block ${blockId}.`);
  };

  const resetMemory = () => {
    setBlocks(initialBlocks);
    setProcessName("P1");
    setProcessSize("120");
    setMessage("Memory has been reset. All blocks are free.");
  };

  return (
    <div className="memory-page">
      <div className="memory-intro">
        <div>
          <h2>Memory Management</h2>
          <p>Allocate, monitor and release memory for running processes.</p>
        </div>
        <span className="memory-tag">MEMORY MODULE</span>
      </div>

      <div className="memory-stats">
        <div className="memory-stat">
          <span>Total Memory</span>
          <h2>{totalMemory} <small>units</small></h2>
        </div>
        <div className="memory-stat">
          <span>Used Memory</span>
          <h2>{usedMemory} <small>units</small></h2>
        </div>
        <div className="memory-stat">
          <span>Free Memory</span>
          <h2>{freeMemory} <small>units</small></h2>
        </div>
        <div className="memory-stat">
          <span>Utilization</span>
          <h2>{utilization.toFixed(1)}%</h2>
        </div>
      </div>

      <section className="memory-panel">
        <div className="memory-panel-heading">
          <div>
            <h3>Memory Allocation</h3>
            <p>Choose an allocation strategy and assign memory to a process.</p>
          </div>
        </div>

        <div className="memory-controls">
          <div className="memory-field">
            <label>Process Name</label>
            <input
              value={processName}
              onChange={(event) => setProcessName(event.target.value)}
              placeholder="e.g. P1"
            />
          </div>

          <div className="memory-field">
            <label>Memory Required (units)</label>
            <input
              type="number"
              min="1"
              value={processSize}
              onChange={(event) => setProcessSize(event.target.value)}
            />
          </div>

          <div className="memory-field">
            <label>Allocation Algorithm</label>
            <select
              value={algorithm}
              onChange={(event) => setAlgorithm(event.target.value)}
            >
              <option>First Fit</option>
              <option>Best Fit</option>
              <option>Worst Fit</option>
            </select>
          </div>

          <button className="memory-allocate" onClick={allocateMemory}>
            + Allocate
          </button>
        </div>

        <div className="memory-message">{message}</div>
      </section>

      <section className="memory-panel">
        <div className="memory-panel-heading">
          <div>
            <h3>Memory Blocks</h3>
            <p>Current allocation status of each memory partition.</p>
          </div>
          <button className="memory-reset" onClick={resetMemory}>
            Reset Memory
          </button>
        </div>

        <div className="memory-blocks">
          {blocks.map((block) => (
            <div
              className={`memory-block ${block.process ? "occupied" : "available"}`}
              key={block.id}
            >
              <div className="memory-block-top">
                <strong>Block {block.id}</strong>
                <span>{block.size} units</span>
              </div>

              {block.process ? (
                <>
                  <div className="memory-process">
                    <strong>{block.process.name}</strong>
                    <span>{block.process.size} units allocated</span>
                  </div>
                  <div className="memory-block-footer">
                    <span>
                      {block.size - block.process.size} units unused
                    </span>
                    <button onClick={() => deallocateMemory(block.id)}>
                      Release
                    </button>
                  </div>
                </>
              ) : (
                <div className="memory-free-label">
                  <span className="free-dot"></span>
                  Available
                </div>
              )}
            </div>
          ))}
        </div>

        <div className="memory-legend">
          <span><i className="legend-used"></i> Allocated</span>
          <span><i className="legend-free"></i> Available</span>
        </div>
      </section>

      <section className="memory-panel">
        <div className="memory-panel-heading">
          <div>
            <h3>Memory Analysis</h3>
            <p>Overview of memory consumption and fragmentation.</p>
          </div>
        </div>

        <div className="memory-utilization">
          <div className="utilization-heading">
            <strong>Memory Utilization</strong>
            <span>{utilization.toFixed(1)}%</span>
          </div>
          <div className="utilization-track">
            <div
              className="utilization-fill"
              style={{ width: `${utilization}%` }}
            ></div>
          </div>
        </div>

        <div className="fragmentation-card">
          <div>
            <strong>Internal Fragmentation</strong>
            <p>Unused space inside allocated partitions.</p>
          </div>
          <strong>{internalFragmentation} units</strong>
        </div>
      </section>
    </div>
  );
}

export default MemoryManagement;
