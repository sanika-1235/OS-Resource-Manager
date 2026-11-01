
const express = require("express");
const cors = require("cors");
const { spawn } = require("child_process");
const path = require("path");

const app = express();

app.use(cors());
app.use(express.json());

app.get("/", (req, res) => {
    res.send("OS Resource Manager Backend is Running!");
});

app.post("/run-cpp", (req, res) => {
    const { processes, resources, available, allocation, maximum } = req.body;

    const input = [
        processes,
        resources,
        ...available,
        ...allocation.flat(),
        ...maximum.flat()
    ].join(" ") + "\n";

    const cppPath = path.join(__dirname, "..", "deadlock.exe");

    const cpp = spawn(cppPath);

    let output = "";
    let errorOutput = "";

    cpp.stdout.on("data", (data) => {
        output += data.toString();
    });

    cpp.stderr.on("data", (data) => {
        errorOutput += data.toString();
    });

    cpp.on("close", (code) => {
        if (code !== 0) {
            return res.status(500).json({
                error: errorOutput || "C++ execution failed"
            });
        }

        res.json({ output });
    });

    cpp.stdin.write(input);
    cpp.stdin.end();
});

const server = app.listen(5000, "127.0.0.1", () => {
    console.log("Backend running at http://127.0.0.1:5000");
});

server.on("error", (error) => {
    console.error("Server error:", error.message);
});
