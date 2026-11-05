(() => {
  "use strict";

  const $ = (id) => document.getElementById(id);

  const domainInput = $("domain-id");
  const domainApplyBtn = $("domain-apply-btn");
  const domainStatus = $("domain-status");

  const cmdTopicInput = $("cmd-topic");
  const stateTopicInput = $("state-topic");
  const topicsList = $("topics-list");
  const cmdDot = $("cmd-dot");
  const stateDot = $("state-dot");

  const connectBtn = $("connect-btn");
  const refreshBtn = $("refresh-btn");
  const degChk = $("deg-chk");
  const statusLine = $("status-line");

  const tableBody = $("joint-table-body");
  const tablePlaceholder = $("table-placeholder");
  const thCmd = $("th-cmd");
  const thActual = $("th-actual");
  const thDiff = $("th-diff");

  const statAvg = $("stat-avg");
  const statMax = $("stat-max");
  const statSamples = $("stat-samples");
  const resetStatsBtn = $("reset-stats-btn");

  const startRecBtn = $("start-rec-btn");
  const stopRecBtn = $("stop-rec-btn");
  const clearRecBtn = $("clear-rec-btn");
  const saveCsvBtn = $("save-csv-btn");
  const recDuration = $("rec-duration");

  let connected = false;
  let dataPollTimer = null;
  let topicsRefreshTimer = null;
  let topicStatusTimer = null;
  let knownJointNames = [];

  const STATUS_TEXT = {
    connected:    ["● Connected  (leader + follower live)", "status-connected"],
    actual_only:  ["● Live  (follower active — leader silent)", "status-waiting"],
    cmd_only:     ["● Live  (command active — leader + follower silent)", "status-waiting"],
    waiting:      ["● Waiting for data…", "status-waiting"],
    disconnected: ["● Disconnected", "status-disconnected"],
    error:        ["● Error", "status-error"],
  };

  function setStatus(state, errorMsg) {
    const [text, cls] = STATUS_TEXT[state] || STATUS_TEXT.disconnected;
    statusLine.textContent = errorMsg ? `⚠ ${errorMsg}` : text;
    statusLine.className = `status ${errorMsg ? "status-error" : cls}`;
  }

  function fmt(v) {
    return v === null || v === undefined ? "—" : v.toFixed(4);
  }

  function diffEmoji(bucket) {
    return { green: "🟢", yellow: "🟡", red: "🔴" }[bucket] || "";
  }

  async function refreshTopics() {
    try {
      const res = await fetch("/api/topics");
      const data = await res.json();
      topicsList.innerHTML = "";
      for (const t of data.topics) {
        const opt = document.createElement("option");
        opt.value = t;
        topicsList.appendChild(opt);
      }
    } catch (e) {
      console.error("refreshTopics failed", e);
    }
  }

  async function refreshTopicStatus() {
    const cmd = cmdTopicInput.value.trim();
    const state = stateTopicInput.value.trim();
    const topics = [cmd, state].filter(Boolean);
    if (topics.length === 0) {
      cmdDot.classList.remove("live");
      stateDot.classList.remove("live");
      return;
    }
    try {
      const res = await fetch(`/api/topic_status?topics=${encodeURIComponent(topics.join(","))}`);
      const status = await res.json();
      cmdDot.classList.toggle("live", !!status[cmd]);
      stateDot.classList.toggle("live", !!status[state]);
    } catch (e) {
      console.error("refreshTopicStatus failed", e);
    }
  }

  function updateUnitHeaders() {
    const unit = degChk.checked ? "deg" : "rad";
    thCmd.textContent = `joint_cmd (${unit})`;
    thActual.textContent = `joint_states (${unit})`;
    thDiff.textContent = `Difference (${unit})`;
  }

  function rebuildTableRows(names) {
    tableBody.innerHTML = "";
    for (const name of names) {
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td class="joint-name">${name}</td>
        <td class="cell-cmd">—</td>
        <td class="cell-actual">—</td>
        <td class="cell-diff">—</td>
      `;
      tableBody.appendChild(tr);
    }
    knownJointNames = names;
  }

  async function pollData() {
    if (!connected) return;
    const degrees = degChk.checked;
    try {
      const res = await fetch(`/api/data?degrees=${degrees}`);
      const data = await res.json();

      setStatus(data.status, data.error);

      const names = data.rows.map((r) => r.name);
      if (names.length === 0) {
        tablePlaceholder.style.display = "block";
        tableBody.innerHTML = "";
        knownJointNames = [];
      } else {
        tablePlaceholder.style.display = "none";
        if (JSON.stringify(names) !== JSON.stringify(knownJointNames)) {
          rebuildTableRows(names);
        }
        const rows = tableBody.children;
        data.rows.forEach((r, i) => {
          const tr = rows[i];
          if (!tr) return;
          tr.querySelector(".cell-cmd").textContent = fmt(r.cmd);
          const diffCell = tr.querySelector(".cell-diff");
          if (r.diff !== null) {
            diffCell.textContent = `${diffEmoji(r.diff_bucket)} ${fmt(r.diff)}`;
            diffCell.className = `cell-diff diff-${r.diff_bucket}`;
          } else {
            diffCell.textContent = "—";
            diffCell.className = "cell-diff diff-muted";
          }
          tr.querySelector(".cell-actual").textContent = fmt(r.actual);
        });
      }

      statAvg.textContent = data.stats.samples ? `${data.stats.avg.toFixed(4)} ${data.unit}` : "—";
      statMax.textContent = data.stats.samples ? `${data.stats.max.toFixed(4)} ${data.unit}` : "—";
      statSamples.textContent = data.stats.samples;

      startRecBtn.disabled = data.recording.active;
      stopRecBtn.disabled = !data.recording.active;
      recDuration.textContent = `Duration: ${data.recording.elapsed_str} | Samples: ${data.recording.samples}`;
    } catch (e) {
      console.error("pollData failed", e);
    }
  }

  async function connect() {
    const cmd_topic = cmdTopicInput.value.trim();
    const state_topic = stateTopicInput.value.trim();
    if (!cmd_topic || !state_topic) {
      alert("Please select both topics before connecting.");
      return;
    }
    const res = await fetch("/api/connect", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ cmd_topic, state_topic }),
    });
    const data = await res.json();
    if (!data.ok) {
      alert(data.error || "Connect failed.");
      return;
    }
    connected = true;
    connectBtn.textContent = "Disconnect";
    connectBtn.classList.add("disconnect-mode");
    cmdTopicInput.disabled = true;
    stateTopicInput.disabled = true;
    setStatus("waiting");
    clearInterval(topicsRefreshTimer);
    dataPollTimer = setInterval(pollData, 100);
  }

  async function disconnect() {
    await fetch("/api/disconnect", { method: "POST" });
    connected = false;
    clearInterval(dataPollTimer);
    dataPollTimer = null;
    connectBtn.textContent = "Connect";
    connectBtn.classList.remove("disconnect-mode");
    cmdTopicInput.disabled = false;
    stateTopicInput.disabled = false;
    setStatus("disconnected");
    tablePlaceholder.style.display = "block";
    tableBody.innerHTML = "";
    knownJointNames = [];
    startRecBtn.disabled = false;
    stopRecBtn.disabled = true;
    topicsRefreshTimer = setInterval(refreshTopics, 5000);
  }

  connectBtn.addEventListener("click", () => (connected ? disconnect() : connect()));
  refreshBtn.addEventListener("click", () => {
    refreshTopics();
    refreshTopicStatus();
  });
  degChk.addEventListener("change", updateUnitHeaders);

  domainApplyBtn.addEventListener("click", async () => {
    if (connected) {
      alert("Please Disconnect first before changing the Domain ID.");
      return;
    }
    const domain_id = parseInt(domainInput.value, 10) || 0;
    const res = await fetch("/api/domain", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ domain_id }),
    });
    const data = await res.json();
    if (!data.ok) {
      alert(data.error || "Failed to apply domain ID.");
      return;
    }
    domainStatus.textContent = `Current: ${domain_id}`;
    refreshTopics();
    refreshTopicStatus();
  });

  resetStatsBtn.addEventListener("click", async () => {
    await fetch("/api/stats/reset", { method: "POST" });
    statAvg.textContent = "—";
    statMax.textContent = "—";
    statSamples.textContent = "0";
  });

  startRecBtn.addEventListener("click", async () => {
    if (!connected) {
      alert("Connect to topics before recording.");
      return;
    }
    const res = await fetch("/api/record/start", { method: "POST" });
    const data = await res.json();
    if (!data.ok) {
      alert(data.error || "Failed to start recording.");
      return;
    }
    startRecBtn.disabled = true;
    stopRecBtn.disabled = false;
  });

  stopRecBtn.addEventListener("click", async () => {
    await fetch("/api/record/stop", { method: "POST" });
    startRecBtn.disabled = false;
    stopRecBtn.disabled = true;
  });

  clearRecBtn.addEventListener("click", async () => {
    await fetch("/api/record/clear", { method: "POST" });
    recDuration.textContent = "Duration: 00:00 | Samples: 0";
    startRecBtn.disabled = false;
    stopRecBtn.disabled = true;
  });

  saveCsvBtn.addEventListener("click", async () => {
    const degrees = degChk.checked;
    const res = await fetch(`/api/record/csv?degrees=${degrees}`);
    if (!res.ok) {
      const data = await res.json().catch(() => ({}));
      alert(data.error || "No samples recorded yet.");
      return;
    }
    const blob = await res.blob();
    const disposition = res.headers.get("Content-Disposition") || "";
    const match = disposition.match(/filename="?([^"]+)"?/);
    const filename = match ? match[1] : "joint_analysis.csv";

    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    a.remove();
    URL.revokeObjectURL(url);
    alert(`✅ Saved as ${filename}`);
  });

  // ── Init ──────────────────────────────────────────────────────────────
  (async function init() {
    updateUnitHeaders();
    domainInput.value = 0;
    try {
      const res = await fetch("/api/domain");
      const data = await res.json();
      domainInput.value = data.domain_id;
      domainStatus.textContent = `Current: ${data.domain_id}`;
    } catch (e) { /* ignore */ }

    await refreshTopics();
    await refreshTopicStatus();
    topicsRefreshTimer = setInterval(refreshTopics, 5000);
    topicStatusTimer = setInterval(refreshTopicStatus, 3000);
  })();
})();
