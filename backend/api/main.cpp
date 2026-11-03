
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "../process/ProcessManager.h"
#include "../cpu/Scheduler.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using json = nlohmann::json;
using namespace std;

ProcessManager manager;
mutex managerMutex;

string stateToString(ProcessState state) {
    switch (state) {
        case ProcessState::NEW: return "NEW";
        case ProcessState::READY: return "READY";
        case ProcessState::RUNNING: return "RUNNING";
        case ProcessState::WAITING: return "WAITING";
        case ProcessState::TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

json processToJson(const Process& p) {
    return {
        {"pid", p.pid},
        {"name", p.name},
        {"arrivalTime", p.arrivalTime},
        {"burstTime", p.burstTime},
        {"remainingTime", p.remainingTime},
        {"priority", p.priority},
        {"state", stateToString(p.state)}
    };
}

json schedulingResultToJson(const SchedulingResult& result) {
    json metrics = json::array();
    json gantt = json::array();
    json decisions = json::array();

    for (const auto& m : result.metrics) {
        metrics.push_back({
            {"pid", m.pid},
            {"completionTime", m.completionTime},
            {"turnaroundTime", m.turnaroundTime},
            {"waitingTime", m.waitingTime},
            {"responseTime", m.responseTime}
        });
    }

    for (const auto& g : result.ganttChart) {
        gantt.push_back({
            {"pid", g.pid},
            {"startTime", g.startTime},
            {"endTime", g.endTime}
        });
    }

    for (const auto& d : result.decisions) {
        decisions.push_back({
            {"time", d.time},
            {"selectedPid", d.selectedPid},
            {"reason", d.reason}
        });
    }

    return {
        {"metrics", metrics},
        {"ganttChart", gantt},
        {"decisions", decisions},
        {"averageTurnaroundTime", result.averageTurnaroundTime},
        {"averageWaitingTime", result.averageWaitingTime},
        {"averageResponseTime", result.averageResponseTime}
    };
}

void setCorsHeaders(const httplib::Request& req,
                    httplib::Response& res) {
    string origin = req.get_header_value("Origin");

    if (origin == "http://localhost:5173" ||
        origin == "http://localhost:5174") {
        res.set_header("Access-Control-Allow-Origin", origin);
    }

    res.set_header("Access-Control-Allow-Methods",
                   "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void sendJson(const httplib::Request& req,
              httplib::Response& res,
              const json& data,
              int status = 200) {
    res.status = status;
    res.set_content(data.dump(), "application/json");
    setCorsHeaders(req, res);
}

int main() {
    httplib::Server server;

    // CORS preflight
    server.Options(R"(.*)", [](const httplib::Request& req,
                               httplib::Response& res) {
        setCorsHeaders(req, res);
    });

    // GET: Fetch all processes
    server.Get("/api/processes", [](const httplib::Request& req,
                                    httplib::Response& res) {
        lock_guard<mutex> lock(managerMutex);

        json result = json::array();

        for (const auto& p : manager.getAllProcesses()) {
            result.push_back(processToJson(p));
        }

        sendJson(req, res, result);
    });

    // POST: Create a process
    server.Post("/api/processes", [](const httplib::Request& req,
                                     httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            string name = body.at("name").get<string>();
            int arrival = body.at("arrivalTime").get<int>();
            int burst = body.at("burstTime").get<int>();
            int priority = body.at("priority").get<int>();

            lock_guard<mutex> lock(managerMutex);

            int newPid = 1;

            for (const auto& p : manager.getAllProcesses()) {
                newPid = max(newPid, p.pid + 1);
            }

            bool created = manager.createProcess(
                newPid, name, arrival, burst, priority
            );

            if (!created) {
                sendJson(req, res,
                    {{"error", "Invalid process details"}}, 400);
                return;
            }

            sendJson(req, res,
                {{"message", "Process created"}, {"pid", newPid}}, 201);
        }
        catch (...) {
            sendJson(req, res,
                {{"error", "Invalid JSON or missing fields"}}, 400);
        }
    });

    // PUT: Update process state
    server.Put(R"(/api/processes/(\d+)/state)",
        [](const httplib::Request& req, httplib::Response& res) {
        try {
            int pid = stoi(req.matches[1].str());
            json body = json::parse(req.body);
            string state = body.at("state").get<string>();

            ProcessState newState;

            if (state == "NEW") newState = ProcessState::NEW;
            else if (state == "READY") newState = ProcessState::READY;
            else if (state == "RUNNING") newState = ProcessState::RUNNING;
            else if (state == "WAITING") newState = ProcessState::WAITING;
            else {
                sendJson(req, res, {{"error", "Invalid state"}}, 400);
                return;
            }

            lock_guard<mutex> lock(managerMutex);

            if (!manager.updateState(pid, newState)) {
                sendJson(req, res,
                    {{"error", "Process not found or invalid state transition"}},
                    400);
                return;
            }

            sendJson(req, res, {{"message", "State updated"}});
        }
        catch (...) {
            sendJson(req, res, {{"error", "Invalid request"}}, 400);
        }
    });

    // DELETE: Terminate a process
    server.Delete(R"(/api/processes/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
        try {
            int pid = stoi(req.matches[1].str());

            lock_guard<mutex> lock(managerMutex);

            if (!manager.terminateProcess(pid)) {
                sendJson(req, res,
                    {{"error", "Process not found or cannot be terminated"}},
                    400);
                return;
            }

            sendJson(req, res, {{"message", "Process terminated"}});
        }
        catch (...) {
            sendJson(req, res, {{"error", "Invalid process ID"}}, 400);
        }
    });

    // POST: Run CPU Scheduling Simulation
    server.Post("/api/scheduling/run",
        [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            string algorithm = body.at("algorithm").get<string>();
            json inputProcesses = body.at("processes");

            if (!inputProcesses.is_array() || inputProcesses.empty()) {
                sendJson(req, res,
                    {{"error", "At least one process is required"}}, 400);
                return;
            }

            vector<CPUProcess> processes;

            for (const auto& p : inputProcesses) {
                CPUProcess process{
                    p.at("pid").get<int>(),
                    p.at("arrivalTime").get<int>(),
                    p.at("burstTime").get<int>(),
                    p.at("priority").get<int>()
                };

                if (process.pid <= 0 ||
                    process.arrivalTime < 0 ||
                    process.burstTime <= 0 ||
                    process.priority < 0) {
                    sendJson(req, res,
                        {{"error", "Invalid process details"}}, 400);
                    return;
                }

                processes.push_back(process);
            }

            SchedulingResult result{};

            if (algorithm == "FCFS") {
                result = runFCFS(processes);
            }
            else if (algorithm == "SJF") {
                result = runSJF(processes);
            }
            else if (algorithm == "SRTF") {
                result = runSRTF(processes);
            }
            else if (algorithm == "Priority") {
                result = runPriority(processes);
            }
            else if (algorithm == "Round Robin") {
                int quantum = body.at("timeQuantum").get<int>();

                if (quantum <= 0) {
                    sendJson(req, res,
                        {{"error", "Time quantum must be positive"}}, 400);
                    return;
                }

                result = runRoundRobin(processes, quantum);
            }
            else {
                sendJson(req, res,
                    {{"error", "Unknown scheduling algorithm"}}, 400);
                return;
            }

            sendJson(req, res, schedulingResultToJson(result));
        }
        catch (const exception& e) {
            sendJson(req, res,
                {{"error", string("Invalid request: ") + e.what()}}, 400);
        }
    });

    cout << "OS Resource Manager API running at http://localhost:8080" << endl;
    cout << "CPU Scheduling endpoint: POST /api/scheduling/run" << endl;
    cout << "Press Ctrl+C to stop the server." << endl;

    server.listen("0.0.0.0", 8080);

    return 0;
}
