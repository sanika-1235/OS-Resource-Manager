
import "./DecisionAnalyzer.css";

const decisions = [
  { time: 0, pid: 1, reason: "First process available in the ready queue." },
  { time: 5, pid: 2, reason: "P1 completed execution. P2 was next by arrival time." },
  { time: 8, pid: 3, reason: "P2 completed execution. P3 was the remaining process." },
];

const waitingTimes = [
  { pid: 1, waiting: 0 },
  { pid: 2, waiting: 4 },
  { pid: 3, waiting: 6 },
];

function DecisionAnalyzer() {
  const averageWaitingTime = 3.33;

  return (
    <div className="analyzer-page">
      <div className="analyzer-intro">
        <div>
          <h2>OS Decision Analyzer</h2>
          <p>
            Understand scheduling decisions and analyze process waiting time.
          </p>
        </div>
        <span className="analyzer-tag">ANALYSIS MODULE</span>
      </div>

      <div className="analyzer-summary">
        <div className="analyzer-summary-card">
          <span>Total Decisions</span>
          <h2>3</h2>
          <small>Scheduling events</small>
        </div>

        <div className="analyzer-summary-card">
          <span>Average Waiting Time</span>
          <h2>{averageWaitingTime}</h2>
          <small>Time units</small>
        </div>

        <div className="analyzer-summary-card">
          <span>Processes Analyzed</span>
          <h2>3</h2>
          <small>Completed processes</small>
        </div>
      </div>

      <section className="analyzer-panel">
        <div className="analyzer-panel-heading">
          <div>
            <h3>Scheduling Decision Log</h3>
            <p>Why was a particular process selected?</p>
          </div>
          <span className="analysis-status">Sample preview</span>
        </div>

        <div className="decision-timeline">
          {decisions.map((decision, index) => (
            <div className="decision-item" key={index}>
              <div className="decision-time">
                <span className="decision-dot"></span>
                <span>Time {decision.time}</span>
              </div>

              <div className="decision-content">
                <div className="decision-top">
                  <strong>Process P{decision.pid} selected</strong>
                  <span className="selected-label">Selected</span>
                </div>
                <p>{decision.reason}</p>
              </div>
            </div>
          ))}
        </div>
      </section>

      <section className="analyzer-panel">
        <div className="analyzer-panel-heading">
          <div>
            <h3>Process Waiting Time Analysis</h3>
            <p>Time spent by each process waiting in the ready queue.</p>
          </div>
        </div>

        <div className="waiting-chart">
          {waitingTimes.map((process) => (
            <div className="waiting-row" key={process.pid}>
              <strong>P{process.pid}</strong>
              <div className="waiting-track">
                <div
                  className="waiting-fill"
                  style={{ width: `${(process.waiting / 6) * 100}%` }}
                ></div>
              </div>
              <span>{process.waiting} units</span>
            </div>
          ))}
        </div>

        <div className="average-waiting">
          <div>
            <span>Average Waiting Time</span>
            <p>Across all analyzed processes</p>
          </div>
          <strong>3.33 <small>units</small></strong>
        </div>
      </section>

      <div className="analyzer-note">
        <strong>Analysis insight</strong>
        <p>
          In FCFS scheduling, processes are executed in arrival order.
          P1 starts immediately, while P2 and P3 wait for earlier processes
          to finish.
        </p>
      </div>
    </div>
  );
}

export default DecisionAnalyzer;
