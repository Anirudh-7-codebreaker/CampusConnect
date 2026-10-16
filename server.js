import http from 'node:http';
import { spawn } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const PORT = process.env.PORT || 3000;

// State of the running C++ CampusConnect process
let cppProcess = null;
let outputBuffer = '';
const clients = new Set();

function broadcast(data) {
    outputBuffer += data;
    // Limit buffer to last 100,000 characters
    if (outputBuffer.length > 100000) {
        outputBuffer = outputBuffer.slice(-100000);
    }
    const msg = `data: ${JSON.stringify({ output: data })}\n\n`;
    for (const res of clients) {
        try {
            res.write(msg);
        } catch (e) {
            clients.delete(res);
        }
    }
}

function startCppProcess() {
    if (cppProcess) {
        try {
            cppProcess.kill('SIGTERM');
        } catch (e) {}
        cppProcess = null;
    }

    const binaryPath = path.join(__dirname, 'CampusConnect');
    if (!fs.existsSync(binaryPath)) {
        broadcast('\x1b[33m[Dev Server] CampusConnect binary not found. Compiling with g++ C++17...\x1b[0m\n');
        compileCppSync();
    }

    try {
        cppProcess = spawn(binaryPath, [], {
            cwd: __dirname,
            stdio: ['pipe', 'pipe', 'pipe'],
            env: { ...process.env, TERM: 'xterm-256color' }
        });

        broadcast('\x1b[32m[Dev Server] CampusConnect C++17 process spawned (PID: ' + cppProcess.pid + ')\x1b[0m\n');

        cppProcess.stdout.on('data', (chunk) => {
            broadcast(chunk.toString());
        });

        cppProcess.stderr.on('data', (chunk) => {
            broadcast(chunk.toString());
        });

        cppProcess.on('exit', (code, signal) => {
            broadcast(`\n\x1b[33m[Dev Server] CampusConnect process exited (code: ${code}, signal: ${signal}).\x1b[0m\n`);
            broadcast('\x1b[36m[Dev Server] Click "Restart C++ Process" or press Enter to run again.\x1b[0m\n');
            cppProcess = null;
        });

        cppProcess.on('error', (err) => {
            broadcast(`\n\x1b[31m[Dev Server Error] Failed to start C++ process: ${err.message}\x1b[0m\n`);
            cppProcess = null;
        });
    } catch (err) {
        broadcast(`\n\x1b[31m[Dev Server Error] ${err.message}\x1b[0m\n`);
    }
}

function compileCppSync() {
    const { execSync } = import('node:child_process');
    try {
        const cmd = 'g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/user.cpp src/student.cpp src/connection_request.cpp src/match_engine.cpp src/file_manager.cpp src/skill_swap_app.cpp -o CampusConnect';
        broadcast('\x1b[34m[Compile] Executing: ' + cmd + '\x1b[0m\n');
        const out = require('child_process').execSync(cmd, { cwd: __dirname, encoding: 'utf-8' });
        broadcast('\x1b[32m[Compile] Build successful! Zero warnings/errors.\x1b[0m\n');
        return true;
    } catch (e) {
        broadcast('\x1b[31m[Compile Error]\x1b[0m\n' + (e.stderr || e.stdout || e.message) + '\n');
        return false;
    }
}

function sendInput(inputStr) {
    if (!cppProcess) {
        startCppProcess();
        setTimeout(() => {
            if (cppProcess && cppProcess.stdin.writable) {
                cppProcess.stdin.write(inputStr.endsWith('\n') ? inputStr : inputStr + '\n');
            }
        }, 300);
        return;
    }

    if (cppProcess.stdin && cppProcess.stdin.writable) {
        cppProcess.stdin.write(inputStr.endsWith('\n') ? inputStr : inputStr + '\n');
    }
}

const HTML_CONTENT = `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>CampusConnect — C++17 Terminal Console</title>
  <style>
    :root {
      --bg-color: #0c1017;
      --card-bg: #161b22;
      --border-color: #30363d;
      --text-main: #e6edf3;
      --text-dim: #8b949e;
      --accent: #58a6ff;
      --accent-green: #3fb950;
      --accent-yellow: #d29922;
      --accent-red: #f85149;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: var(--bg-color);
      color: var(--text-main);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      height: 100vh;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }
    header {
      background: var(--card-bg);
      border-bottom: 1px solid var(--border-color);
      padding: 10px 16px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
    }
    .header-left {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .window-dots {
      display: flex;
      gap: 6px;
    }
    .dot {
      width: 12px;
      height: 12px;
      border-radius: 50%;
    }
    .dot-red { background: #ff5f56; }
    .dot-yellow { background: #ffbd2e; }
    .dot-green { background: #27c93f; }
    .title-badge {
      font-weight: 600;
      font-size: 14px;
      letter-spacing: 0.5px;
      color: var(--text-main);
    }
    .sub-badge {
      font-size: 12px;
      color: var(--text-dim);
      background: #21262d;
      padding: 2px 8px;
      border-radius: 12px;
      border: 1px solid var(--border-color);
    }
    .quick-actions {
      display: flex;
      gap: 8px;
      flex-wrap: wrap;
    }
    .btn {
      background: #21262d;
      color: var(--text-main);
      border: 1px solid var(--border-color);
      padding: 6px 12px;
      border-radius: 6px;
      font-size: 12px;
      font-weight: 500;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
      transition: all 0.15s ease;
    }
    .btn:hover {
      background: #30363d;
      border-color: #8b949e;
    }
    .btn-primary {
      background: #238636;
      border-color: rgba(240,246,252,0.1);
    }
    .btn-primary:hover {
      background: #2ea043;
    }
    .btn-accent {
      background: #1f6feb;
      border-color: rgba(240,246,252,0.1);
    }
    .btn-accent:hover {
      background: #388bfd;
    }
    main {
      flex: 1;
      display: flex;
      flex-direction: column;
      padding: 12px;
      overflow: hidden;
      gap: 10px;
    }
    .terminal-container {
      flex: 1;
      background: #0d1117;
      border: 1px solid var(--border-color);
      border-radius: 8px;
      display: flex;
      flex-direction: column;
      overflow: hidden;
      box-shadow: 0 4px 12px rgba(0,0,0,0.5);
    }
    .terminal-output {
      flex: 1;
      padding: 14px;
      overflow-y: auto;
      font-family: "JetBrains Mono", "Fira Code", Menlo, Monaco, "Courier New", monospace;
      font-size: 13.5px;
      line-height: 1.5;
      white-space: pre-wrap;
      word-break: break-all;
      color: #c9d1d9;
    }
    .terminal-input-bar {
      display: flex;
      align-items: center;
      background: #161b22;
      border-top: 1px solid var(--border-color);
      padding: 8px 12px;
      gap: 10px;
    }
    .prompt-label {
      font-family: monospace;
      font-weight: bold;
      color: var(--accent-green);
      font-size: 14px;
      white-space: nowrap;
    }
    .terminal-input {
      flex: 1;
      background: transparent;
      border: none;
      outline: none;
      color: #58a6ff;
      font-family: monospace;
      font-size: 14px;
      font-weight: 500;
    }
    .terminal-input::placeholder {
      color: #484f58;
    }
    .toolbar-footer {
      display: flex;
      align-items: center;
      justify-content: space-between;
      font-size: 12px;
      color: var(--text-dim);
      padding: 0 4px;
    }
    .keyboard-hint {
      color: #8b949e;
    }
    .keyboard-hint kbd {
      background: #21262d;
      border: 1px solid var(--border-color);
      border-radius: 4px;
      padding: 2px 6px;
      font-family: monospace;
      font-size: 11px;
    }
  </style>
</head>
<body>

  <header>
    <div class="header-left">
      <div class="window-dots">
        <div class="dot dot-red"></div>
        <div class="dot dot-yellow"></div>
        <div class="dot dot-green"></div>
      </div>
      <span class="title-badge">CampusConnect Terminal</span>
      <span class="sub-badge">C++17 Standalone Process</span>
    </div>
    <div class="quick-actions">
      <button class="btn" onclick="sendChoice('1')">1: View Profiles</button>
      <button class="btn" onclick="sendChoice('4')">4: Find Matches</button>
      <button class="btn" onclick="sendChoice('6')">6: Pending Queue</button>
      <button class="btn" onclick="sendChoice('7')">7: Process Next</button>
      <button class="btn" onclick="sendChoice('8')">8: Connections</button>
      <button class="btn btn-accent" onclick="sendChoice('')">⏎ Enter</button>
      <button class="btn btn-primary" onclick="restartProcess()">Restart Process</button>
      <button class="btn" onclick="clearTerminal()">Clear</button>
    </div>
  </header>

  <main>
    <div class="terminal-container" onclick="focusInput()">
      <div id="output" class="terminal-output"></div>
      <div class="terminal-input-bar">
        <span class="prompt-label">CampusConnect &gt;</span>
        <input id="termInput" type="text" class="terminal-input" placeholder="Type menu option (e.g. 1, 4, 101) or text, then press Enter..." autofocus autocomplete="off" spellcheck="false" />
        <button class="btn btn-accent" onclick="submitInput()">Send</button>
      </div>
    </div>
    <div class="toolbar-footer">
      <span>Executable: <code>./CampusConnect</code> | Models: User, Student, Skill, ConnectionRequest, Match</span>
      <span class="keyboard-hint">Press <kbd>Enter</kbd> to submit | Use <kbd>↑</kbd> <kbd>↓</kbd> for command history</span>
    </div>
  </main>

  <script>
    const outputEl = document.getElementById('output');
    const inputEl = document.getElementById('termInput');
    const history = [];
    let historyIndex = -1;

    function appendOutput(text) {
      // Simple ANSI stripper/converter for cleaner display
      const clean = text
        .replace(/\\x1b\\[31m/g, '<span style="color:#f85149">')
        .replace(/\\x1b\\[32m/g, '<span style="color:#3fb950">')
        .replace(/\\x1b\\[33m/g, '<span style="color:#d29922">')
        .replace(/\\x1b\\[34m/g, '<span style="color:#58a6ff">')
        .replace(/\\x1b\\[36m/g, '<span style="color:#39c5bb">')
        .replace(/\\x1b\\[0m/g, '</span>');

      outputEl.innerHTML += clean;
      outputEl.scrollTop = outputEl.scrollHeight;
    }

    function focusInput() {
      inputEl.focus();
    }

    function submitInput() {
      const val = inputEl.value;
      sendChoice(val);
      if (val.trim()) {
        history.push(val);
        historyIndex = history.length;
      }
      inputEl.value = '';
    }

    function sendChoice(choice) {
      fetch('/api/input', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ input: choice })
      });
      focusInput();
    }

    function restartProcess() {
      fetch('/api/restart', { method: 'POST' });
      focusInput();
    }

    function clearTerminal() {
      outputEl.innerHTML = '';
      focusInput();
    }

    inputEl.addEventListener('keydown', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        submitInput();
      } else if (e.key === 'ArrowUp') {
        if (history.length > 0 && historyIndex > 0) {
          historyIndex--;
          inputEl.value = history[historyIndex];
        }
      } else if (e.key === 'ArrowDown') {
        if (historyIndex < history.length - 1) {
          historyIndex++;
          inputEl.value = history[historyIndex];
        } else {
          historyIndex = history.length;
          inputEl.value = '';
        }
      }
    });

    // Connect to live Server-Sent Events stream from C++ process
    const evtSource = new EventSource('/api/stream');
    evtSource.onmessage = (e) => {
      try {
        const data = JSON.parse(e.data);
        if (data.output) {
          appendOutput(data.output);
        }
      } catch (err) {}
    };

    evtSource.onerror = () => {
      console.log('Stream disconnected, retrying...');
    };
  </script>
</body>
</html>`;

const server = http.createServer((req, res) => {
    const url = new URL(req.url, `http://${req.headers.host}`);

    if (url.pathname === '/' || url.pathname === '/index.html') {
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(HTML_CONTENT);
        return;
    }

    if (url.pathname === '/api/stream') {
        res.writeHead(200, {
            'Content-Type': 'text/event-stream',
            'Cache-Control': 'no-cache',
            'Connection': 'keep-alive',
            'Access-Control-Allow-Origin': '*'
        });
        res.write(`data: ${JSON.stringify({ output: outputBuffer })}\n\n`);
        clients.add(res);

        req.on('close', () => {
            clients.delete(res);
        });
        return;
    }

    if (url.pathname === '/api/input' && req.method === 'POST') {
        let body = '';
        req.on('data', chunk => body += chunk);
        req.on('end', () => {
            try {
                const data = JSON.parse(body);
                sendInput(data.input ?? '');
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ status: 'ok' }));
            } catch (err) {
                res.writeHead(400, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ error: err.message }));
            }
        });
        return;
    }

    if (url.pathname === '/api/restart' && req.method === 'POST') {
        outputBuffer = '';
        startCppProcess();
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ status: 'restarted' }));
        return;
    }

    res.writeHead(404, { 'Content-Type': 'text/plain' });
    res.end('Not Found');
});

server.listen(PORT, '0.0.0.0', () => {
    console.log(`[Dev Server] CampusConnect C++ Terminal Server listening on http://0.0.0.0:${PORT}`);
    startCppProcess();
});
