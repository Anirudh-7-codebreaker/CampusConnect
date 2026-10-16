const http = require('http');
const { spawn, exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const PORT = 3000;
const HOST = '0.0.0.0';

let activeChild = null;
const eventClients = new Set();
let sessionOutputBuffer = '';

function broadcast(data) {
  sessionOutputBuffer += data;
  if (sessionOutputBuffer.length > 50000) {
    sessionOutputBuffer = sessionOutputBuffer.slice(-40000);
  }
  const payload = `data: ${JSON.stringify({ text: data })}\n\n`;
  for (const client of eventClients) {
    try {
      client.write(payload);
    } catch (e) {
      eventClients.delete(client);
    }
  }
}

function startProcess() {
  if (activeChild) {
    try {
      activeChild.kill();
    } catch (e) {}
    activeChild = null;
  }

  sessionOutputBuffer = '';
  broadcast('\r\n[Starting SkillSwap C++ Interactive Terminal Engine...]\r\n');

  const binaryPath = path.join(__dirname, 'SkillSwap');
  if (!fs.existsSync(binaryPath)) {
    broadcast('[!] SkillSwap binary not found. Compiling first...\r\n');
    exec('g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/user.cpp src/student.cpp src/connection_request.cpp src/match_engine.cpp src/file_manager.cpp src/skill_swap_app.cpp -o SkillSwap', (err, stdout, stderr) => {
      if (err) {
        broadcast(`[Build Failed] ${stderr || err.message}\r\n`);
        return;
      }
      broadcast('[Build Succeeded] Launching binary...\r\n');
      spawnBinary();
    });
  } else {
    spawnBinary();
  }
}

function spawnBinary() {
  const binaryPath = path.join(__dirname, 'SkillSwap');
  activeChild = spawn(binaryPath, [], {
    cwd: __dirname,
    env: process.env,
    stdio: ['pipe', 'pipe', 'pipe']
  });

  activeChild.stdout.on('data', (chunk) => {
    broadcast(chunk.toString());
  });

  activeChild.stderr.on('data', (chunk) => {
    broadcast(chunk.toString());
  });

  activeChild.on('close', (code) => {
    broadcast(`\r\n[SkillSwap process exited with code ${code}]\r\n`);
    activeChild = null;
  });

  activeChild.on('error', (err) => {
    broadcast(`\r\n[Process error: ${err.message}]\r\n`);
    activeChild = null;
  });
}

const server = http.createServer((req, res) => {
  const parsedUrl = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
  const pathname = parsedUrl.pathname;

  // CORS headers
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    res.writeHead(204);
    res.end();
    return;
  }

  // SSE Stream for live terminal output
  if (pathname === '/api/terminal/stream') {
    res.writeHead(200, {
      'Content-Type': 'text/event-stream',
      'Cache-Control': 'no-cache',
      'Connection': 'keep-alive',
    });
    res.write(`data: ${JSON.stringify({ text: sessionOutputBuffer })}\n\n`);
    eventClients.add(res);

    req.on('close', () => {
      eventClients.delete(res);
    });
    return;
  }

  // Send input to running SkillSwap process
  if (pathname === '/api/terminal/input' && req.method === 'POST') {
    let body = '';
    req.on('data', chunk => { body += chunk; });
    req.on('end', () => {
      try {
        const { input } = JSON.parse(body);
        if (activeChild && activeChild.stdin && !activeChild.stdin.destroyed) {
          activeChild.stdin.write(input + '\n');
          res.writeHead(200, { 'Content-Type': 'application/json' });
          res.end(JSON.stringify({ status: 'ok' }));
        } else {
          // Restart process if it exited
          startProcess();
          setTimeout(() => {
            if (activeChild && activeChild.stdin) {
              activeChild.stdin.write(input + '\n');
            }
          }, 300);
          res.writeHead(200, { 'Content-Type': 'application/json' });
          res.end(JSON.stringify({ status: 'restarted_and_sent' }));
        }
      } catch (e) {
        res.writeHead(400, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: e.message }));
      }
    });
    return;
  }

  // Restart / Reset Process
  if (pathname === '/api/terminal/restart' && req.method === 'POST') {
    startProcess();
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ status: 'restarted' }));
    return;
  }

  // Rebuild C++ binary
  if (pathname === '/api/terminal/build' && req.method === 'POST') {
    broadcast('\r\n[Running g++ -std=c++17 -Wall -Wextra -g -I include src/*.cpp -o SkillSwap ...]\r\n');
    exec('g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/user.cpp src/student.cpp src/connection_request.cpp src/match_engine.cpp src/file_manager.cpp src/skill_swap_app.cpp -o SkillSwap', (err, stdout, stderr) => {
      if (err) {
        broadcast(`[Build Error]\r\n${stderr || err.message}\r\n`);
        res.writeHead(500, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ success: false, error: stderr || err.message }));
      } else {
        broadcast('[✓ Compilation Successful! Zero errors, zero warnings.]\r\n');
        startProcess();
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ success: true, message: 'Build succeeded' }));
      }
    });
    return;
  }

  // View data or doc files
  if (pathname.startsWith('/api/file')) {
    const fileParam = parsedUrl.searchParams.get('path');
    const allowed = ['data/students.txt', 'data/requests.txt', 'data/README.txt', 'docs/architecture.md', 'docs/data_structures.md', 'README.md'];
    if (allowed.includes(fileParam)) {
      const full = path.join(__dirname, fileParam);
      if (fs.existsSync(full)) {
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(fs.readFileSync(full, 'utf8'));
        return;
      }
    }
    res.writeHead(404, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: 'File not found or access restricted' }));
    return;
  }

  // Serve Main Web UI (Interactive Web Terminal)
  if (pathname === '/' || pathname === '/index.html') {
    res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
    res.end(renderWebTerminalHTML());
    return;
  }

  res.writeHead(404);
  res.end('Not Found');
});

function renderWebTerminalHTML() {
  return `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>SkillSwap — C++17 Terminal Prototype (40% Milestone)</title>
  <style>
    :root {
      --bg-primary: #0a0e17;
      --bg-panel: #111827;
      --bg-header: #1f2937;
      --border-color: #374151;
      --accent-blue: #3b82f6;
      --accent-green: #10b981;
      --accent-amber: #f59e0b;
      --text-main: #f3f4f6;
      --text-dim: #9ca3af;
      --term-bg: #030712;
      --term-green: #34d399;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      background: var(--bg-primary);
      color: var(--text-main);
      height: 100vh;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }
    header {
      background: var(--bg-panel);
      border-bottom: 1px solid var(--border-color);
      padding: 10px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-shrink: 0;
    }
    .brand {
      display: flex;
      align-items: center;
      gap: 12px;
    }
    .badge-cpp {
      background: #1e3a8a;
      color: #93c5fd;
      padding: 3px 8px;
      border-radius: 4px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.5px;
    }
    .badge-status {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 12px;
      color: var(--accent-green);
      background: rgba(16, 185, 129, 0.1);
      padding: 4px 10px;
      border-radius: 20px;
      border: 1px solid rgba(16, 185, 129, 0.3);
    }
    .dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--accent-green);
      box-shadow: 0 0 8px var(--accent-green);
    }
    .action-bar {
      display: flex;
      gap: 8px;
      flex-wrap: wrap;
    }
    button {
      background: #2563eb;
      color: #fff;
      border: 1px solid #1d4ed8;
      border-radius: 6px;
      padding: 6px 14px;
      font-size: 12px;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
      transition: all 0.15s ease;
    }
    button:hover { background: #1d4ed8; transform: translateY(-1px); }
    button.secondary {
      background: #374151;
      border-color: #4b5563;
      color: #e5e7eb;
    }
    button.secondary:hover { background: #4b5563; }
    button.success {
      background: #059669;
      border-color: #047857;
    }
    button.success:hover { background: #047857; }
    
    .main-container {
      display: flex;
      flex: 1;
      overflow: hidden;
    }
    .terminal-section {
      flex: 1;
      display: flex;
      flex-direction: column;
      background: var(--term-bg);
      border-right: 1px solid var(--border-color);
      overflow: hidden;
    }
    .term-header {
      background: #0f172a;
      padding: 8px 16px;
      border-bottom: 1px solid #1e293b;
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      color: var(--text-dim);
    }
    .terminal-screen {
      flex: 1;
      padding: 16px;
      overflow-y: auto;
      font-family: "JetBrains Mono", "Courier New", Consolas, monospace;
      font-size: 13px;
      line-height: 1.5;
      color: #e2e8f0;
      white-space: pre-wrap;
      word-break: break-word;
    }
    .input-bar {
      background: #0f172a;
      border-top: 1px solid #1e293b;
      padding: 10px 16px;
      display: flex;
      gap: 10px;
      align-items: center;
    }
    .prompt-label {
      font-family: monospace;
      color: var(--term-green);
      font-weight: bold;
    }
    .cmd-input {
      flex: 1;
      background: #020617;
      border: 1px solid #334155;
      color: #f8fafc;
      font-family: monospace;
      font-size: 14px;
      padding: 8px 12px;
      border-radius: 6px;
      outline: none;
    }
    .cmd-input:focus { border-color: var(--accent-blue); box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.25); }
    
    .sidebar {
      width: 380px;
      background: var(--bg-panel);
      display: flex;
      flex-direction: column;
      overflow-y: auto;
    }
    .sidebar-section {
      padding: 16px;
      border-bottom: 1px solid var(--border-color);
    }
    .section-title {
      font-size: 12px;
      font-weight: 700;
      text-transform: uppercase;
      letter-spacing: 0.5px;
      color: var(--accent-blue);
      margin-bottom: 12px;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .quick-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
    }
    .quick-btn {
      text-align: left;
      padding: 8px 10px;
      font-size: 11px;
      background: #1f2937;
      border: 1px solid #374151;
      border-radius: 6px;
      color: #e5e7eb;
      cursor: pointer;
      line-height: 1.3;
    }
    .quick-btn:hover {
      background: #374151;
      border-color: #60a5fa;
      color: #fff;
    }
    .key-number {
      color: var(--accent-amber);
      font-weight: bold;
    }
    .doc-viewer {
      background: #030712;
      border: 1px solid var(--border-color);
      border-radius: 6px;
      padding: 10px;
      max-height: 200px;
      overflow-y: auto;
      font-size: 11px;
      font-family: monospace;
      color: #cbd5e1;
      white-space: pre-wrap;
    }
  </style>
</head>
<body>
  <header>
    <div class="brand">
      <h2 style="font-size: 18px; font-weight: 800; letter-spacing: -0.5px;">SkillSwap</h2>
      <span class="badge-cpp">C++17 Terminal App</span>
      <span class="badge-status"><span class="dot"></span> C++ Binary Ready</span>
    </div>
    <div class="action-bar">
      <button onclick="rebuildCPP()" class="secondary">🔨 Recompile C++</button>
      <button onclick="restartTerminal()" class="success">🔄 Restart Session</button>
      <button onclick="sendKey('0')" class="secondary">🚪 Exit App (0)</button>
    </div>
  </header>

  <div class="main-container">
    <section class="terminal-section">
      <div class="term-header">
        <span>Terminal Emulator &bull; /app/applet/SkillSwap (g++ 12.3.0 C++17)</span>
        <span id="term-status" style="color: #10b981;">Online &bull; Interactive</span>
      </div>
      <div id="terminal" class="terminal-screen">Loading SkillSwap Terminal...</div>
      <div class="input-bar">
        <span class="prompt-label">SkillSwap&gt;</span>
        <input type="text" id="command-input" class="cmd-input" placeholder="Enter option (e.g., 1, 4, 101) or press buttons on the right..." autofocus />
        <button onclick="sendInput()">Send</button>
      </div>
    </section>

    <aside class="sidebar">
      <div class="sidebar-section">
        <div class="section-title">⚡ Interactive Menu Shortcuts</div>
        <div class="quick-grid">
          <button class="quick-btn" onclick="sendKey('1')"><span class="key-number">[1]</span> View All Profiles</button>
          <button class="quick-btn" onclick="sendKey('2')"><span class="key-number">[2]</span> Inspect Profile</button>
          <button class="quick-btn" onclick="sendKey('4')"><span class="key-number">[4]</span> Find Skill Matches</button>
          <button class="quick-btn" onclick="sendKey('5')"><span class="key-number">[5]</span> Send Request</button>
          <button class="quick-btn" onclick="sendKey('6')"><span class="key-number">[6]</span> View Pending (Queue)</button>
          <button class="quick-btn" onclick="sendKey('7')"><span class="key-number">[7]</span> Process Request</button>
          <button class="quick-btn" onclick="sendKey('8')"><span class="key-number">[8]</span> Active Connections</button>
          <button class="quick-btn" onclick="sendKey('9')"><span class="key-number">[9]</span> Save Data to Disk</button>
        </div>
      </div>

      <div class="sidebar-section">
        <div class="section-title">🎯 Guided Walkthrough Demos</div>
        <div style="display: flex; flex-direction: column; gap: 6px;">
          <button class="quick-btn" onclick="runMacro(['4', '101', '0'])">
            <strong>▶ Demo 1:</strong> Match Candidates for Student #101 (priority_queue Max-Heap)
          </button>
          <button class="quick-btn" onclick="runMacro(['2', '1', '102', ''])">
            <strong>▶ Demo 2:</strong> Inspect Priya Patel #102 (Polymorphism via User&)
          </button>
          <button class="quick-btn" onclick="runMacro(['6', '', '7', '1', ''])">
            <strong>▶ Demo 3:</strong> Process FIFO Request #1 from Queue & Accept
          </button>
          <button class="quick-btn" onclick="runMacro(['8', '101', '0'])">
            <strong>▶ Demo 4:</strong> View Active Peer Connections (std::list)
          </button>
        </div>
      </div>

      <div class="sidebar-section">
        <div class="section-title">📂 Persistent Data Files</div>
        <div style="display: flex; gap: 6px; margin-bottom: 8px;">
          <button class="secondary" style="font-size: 11px; padding: 4px 8px;" onclick="loadFile('data/students.txt')">students.txt</button>
          <button class="secondary" style="font-size: 11px; padding: 4px 8px;" onclick="loadFile('data/requests.txt')">requests.txt</button>
          <button class="secondary" style="font-size: 11px; padding: 4px 8px;" onclick="loadFile('docs/data_structures.md')">data_structures.md</button>
        </div>
        <div id="file-viewer" class="doc-viewer">Click a button above to inspect file contents stored on disk.</div>
      </div>
    </aside>
  </div>

  <script>
    const term = document.getElementById('terminal');
    const inputField = document.getElementById('command-input');
    let autoScroll = true;

    // Connect to SSE stream
    const evtSource = new EventSource('/api/terminal/stream');
    evtSource.onmessage = function(e) {
      const data = JSON.parse(e.data);
      if (data.text) {
        term.textContent += data.text;
        if (autoScroll) {
          term.scrollTop = term.scrollHeight;
        }
      }
    };
    evtSource.onerror = function() {
      document.getElementById('term-status').textContent = 'Reconnecting...';
      document.getElementById('term-status').style.color = '#f59e0b';
    };
    evtSource.onopen = function() {
      document.getElementById('term-status').textContent = 'Online • Interactive';
      document.getElementById('term-status').style.color = '#10b981';
    };

    function sendInput() {
      const text = inputField.value;
      inputField.value = '';
      fetch('/api/terminal/input', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ input: text })
      });
    }

    function sendKey(val) {
      fetch('/api/terminal/input', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ input: val })
      });
    }

    inputField.addEventListener('keydown', function(e) {
      if (e.key === 'Enter') {
        sendInput();
      }
    });

    function restartTerminal() {
      term.textContent = '';
      fetch('/api/terminal/restart', { method: 'POST' });
    }

    function rebuildCPP() {
      fetch('/api/terminal/build', { method: 'POST' });
    }

    function runMacro(inputs) {
      let delay = 0;
      inputs.forEach(inp => {
        setTimeout(() => {
          sendKey(inp);
        }, delay);
        delay += 600;
      });
    }

    function loadFile(filePath) {
      const viewer = document.getElementById('file-viewer');
      viewer.textContent = 'Loading ' + filePath + '...';
      fetch('/api/file?path=' + encodeURIComponent(filePath))
        .then(r => r.text())
        .then(content => {
          viewer.textContent = content;
        })
        .catch(err => {
          viewer.textContent = 'Error: ' + err.message;
        });
    }
  </script>
</body>
</html>`;
}

// Automatically start process on boot
startProcess();

server.listen(PORT, HOST, () => {
  console.log(`SkillSwap dev server is listening at http://${HOST}:${PORT}`);
});
