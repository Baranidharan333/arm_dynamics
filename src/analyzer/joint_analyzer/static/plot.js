(() => {
  const canvas = document.getElementById("chart");
  const ctx = canvas.getContext("2d");
  const legendEl = document.getElementById("legend");
  const tooltipEl = document.getElementById("tooltip");
  const windowLabelEl = document.getElementById("window-label");
  const tableToggle = document.getElementById("table-toggle");
  const tableEl = document.getElementById("value-table");
  const tableBody = tableEl.querySelector("tbody");

  const PAD = { top: 16, right: 16, bottom: 26, left: 56 };
  const TEXT_MUTED = getComputedStyle(document.documentElement).getPropertyValue("--muted").trim();
  const TEXT_PRIMARY = getComputedStyle(document.documentElement).getPropertyValue("--text").trim();
  const GRID_COLOR = "#2a2a3e";
  const SURFACE = "#181825";

  let seriesMeta = [];   // [{id, path, color, topic, field, error}]
  let latest = null;     // last /api/data payload
  let windowSec = 5.0;
  let hoverX = null;     // canvas-space x while hovering, else null
  let hiddenSeries = new Set();   // series ids toggled off via the legend

  function resizeCanvas() {
    const rect = canvas.getBoundingClientRect();
    const dpr = window.devicePixelRatio || 1;
    canvas.width = Math.max(1, Math.round(rect.width * dpr));
    canvas.height = Math.max(1, Math.round(rect.height * dpr));
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    draw();
  }

  function niceStep(range, targetTicks) {
    if (!(range > 0)) return 1;
    const raw = range / targetTicks;
    const mag = Math.pow(10, Math.floor(Math.log10(raw)));
    const norm = raw / mag;
    let step;
    if (norm < 1.5) step = 1;
    else if (norm < 3) step = 2;
    else if (norm < 7) step = 5;
    else step = 10;
    return step * mag;
  }

  function plotArea() {
    const rect = canvas.getBoundingClientRect();
    return {
      x0: PAD.left,
      y0: PAD.top,
      x1: rect.width - PAD.right,
      y1: rect.height - PAD.bottom,
      w: rect.width - PAD.left - PAD.right,
      h: rect.height - PAD.top - PAD.bottom,
    };
  }

  function nearestSample(series, targetT) {
    const t = series.t;
    if (!t.length) return null;
    let lo = 0, hi = t.length - 1;
    if (targetT <= t[0]) return { t: t[0], v: series.v[0] };
    if (targetT >= t[hi]) return { t: t[hi], v: series.v[hi] };
    while (hi - lo > 1) {
      const mid = (lo + hi) >> 1;
      if (t[mid] < targetT) lo = mid; else hi = mid;
    }
    const i = (targetT - t[lo] < t[hi] - targetT) ? lo : hi;
    return { t: t[i], v: series.v[i] };
  }

  function draw() {
    const rect = canvas.getBoundingClientRect();
    ctx.clearRect(0, 0, rect.width, rect.height);
    ctx.fillStyle = SURFACE;
    ctx.fillRect(0, 0, rect.width, rect.height);

    const area = plotArea();

    if (!latest || !seriesMeta.length) {
      ctx.fillStyle = TEXT_MUTED;
      ctx.font = "12px monospace";
      ctx.fillText("waiting for data...", area.x0, area.y0 + 16);
      return;
    }

    const now = latest.now;
    const tMin = now - windowSec;
    const tMax = now;

    let vMin = Infinity, vMax = -Infinity;
    latest.series.forEach((s, idx) => {
      if (hiddenSeries.has(idx)) return;
      for (let i = 0; i < s.t.length; i++) {
        if (s.t[i] < tMin) continue;
        if (s.v[i] < vMin) vMin = s.v[i];
        if (s.v[i] > vMax) vMax = s.v[i];
      }
    });
    if (!isFinite(vMin) || !isFinite(vMax)) { vMin = -1; vMax = 1; }
    if (vMax - vMin < 1e-6) { vMin -= 1; vMax += 1; }
    const pad = (vMax - vMin) * 0.12;
    vMin -= pad; vMax += pad;

    const xOf = (t) => area.x0 + ((t - tMin) / (tMax - tMin)) * area.w;
    const yOf = (v) => area.y1 - ((v - vMin) / (vMax - vMin)) * area.h;

    // Gridlines - recessive, hairline, one step off the surface color.
    ctx.strokeStyle = GRID_COLOR;
    ctx.lineWidth = 1;
    ctx.font = "10px monospace";
    ctx.fillStyle = TEXT_MUTED;

    const yStep = niceStep(vMax - vMin, 5);
    const yStart = Math.ceil(vMin / yStep) * yStep;
    for (let v = yStart; v <= vMax; v += yStep) {
      const y = Math.round(yOf(v)) + 0.5;
      ctx.beginPath();
      ctx.moveTo(area.x0, y);
      ctx.lineTo(area.x1, y);
      ctx.stroke();
      ctx.fillText(v.toFixed(2), 4, y + 3);
    }

    const xStep = niceStep(windowSec, 5);
    for (let dt = 0; dt <= windowSec + 1e-9; dt += xStep) {
      const t = tMax - dt;
      const x = Math.round(xOf(t)) + 0.5;
      ctx.beginPath();
      ctx.moveTo(x, area.y0);
      ctx.lineTo(x, area.y1);
      ctx.stroke();
      const label = dt < 1e-9 ? "now" : `-${dt.toFixed(dt < 1 ? 1 : 0)}s`;
      const w = ctx.measureText(label).width;
      ctx.fillText(label, Math.min(Math.max(x - w / 2, area.x0), area.x1 - w), area.y1 + 14);
    }

    // Series lines.
    const endPoints = [];
    latest.series.forEach((s, i) => {
      const meta = seriesMeta[i];
      if (!meta || s.t.length < 1 || hiddenSeries.has(i)) return;

      ctx.strokeStyle = meta.color;
      ctx.lineWidth = 2;
      ctx.lineJoin = "round";
      ctx.lineCap = "round";
      ctx.beginPath();
      let started = false;
      for (let k = 0; k < s.t.length; k++) {
        if (s.t[k] < tMin) continue;
        const x = xOf(s.t[k]);
        const y = yOf(s.v[k]);
        if (!started) { ctx.moveTo(x, y); started = true; }
        else ctx.lineTo(x, y);
      }
      if (started) ctx.stroke();

      const last = s.t.length - 1;
      if (last >= 0 && s.t[last] >= tMin) {
        endPoints.push({
          x: xOf(s.t[last]), y: yOf(s.v[last]),
          color: meta.color, value: s.v[last],
        });
      }
    });

    // End-dots (ring in surface color for legibility) + value labels,
    // nudged apart vertically when they'd collide.
    endPoints.sort((a, b) => a.y - b.y);
    for (let i = 1; i < endPoints.length; i++) {
      if (endPoints[i].y - endPoints[i - 1].y < 14) {
        endPoints[i].y = endPoints[i - 1].y + 14;
      }
    }
    for (const p of endPoints) {
      ctx.beginPath();
      ctx.arc(p.x, p.y, 5, 0, Math.PI * 2);
      ctx.fillStyle = SURFACE;
      ctx.fill();
      ctx.beginPath();
      ctx.arc(p.x, p.y, 4, 0, Math.PI * 2);
      ctx.fillStyle = p.color;
      ctx.fill();

      ctx.fillStyle = TEXT_PRIMARY;
      ctx.font = "11px monospace";
      ctx.fillText(p.value.toFixed(4), Math.min(p.x + 8, area.x1 - 52), p.y + 4);
    }

    // Crosshair + tooltip.
    if (hoverX !== null && hoverX >= area.x0 && hoverX <= area.x1) {
      const hoverT = tMin + ((hoverX - area.x0) / area.w) * (tMax - tMin);

      ctx.strokeStyle = "#45475a";
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(Math.round(hoverX) + 0.5, area.y0);
      ctx.lineTo(Math.round(hoverX) + 0.5, area.y1);
      ctx.stroke();

      renderTooltip(hoverT, hoverX, area, latest, seriesMeta, xOf, yOf);
    } else {
      tooltipEl.hidden = true;
    }

    windowLabelEl.textContent = `window: ${windowSec.toFixed(1)}s`;
  }

  function renderTooltip(hoverT, hoverX, area, data, meta, xOf, yOf) {
    tooltipEl.hidden = false;
    tooltipEl.innerHTML = "";

    const timeRow = document.createElement("div");
    timeRow.className = "tt-time";
    timeRow.textContent = `t = ${hoverT.toFixed(2)}s`;
    tooltipEl.appendChild(timeRow);

    data.series.forEach((s, i) => {
      const m = meta[i];
      if (!m || hiddenSeries.has(i)) return;
      const sample = nearestSample(s, hoverT);

      const row = document.createElement("div");
      row.className = "tt-row";

      const swatch = document.createElement("span");
      swatch.className = "swatch";
      swatch.style.background = m.color;
      row.appendChild(swatch);

      const label = document.createElement("span");
      label.className = "tt-label";
      label.textContent = m.path;
      row.appendChild(label);

      const value = document.createElement("span");
      value.className = "tt-value";
      value.textContent = sample ? sample.v.toFixed(4) : "--";
      row.appendChild(value);

      tooltipEl.appendChild(row);
    });

    const wrapRect = canvas.parentElement.getBoundingClientRect();
    const canvasRect = canvas.getBoundingClientRect();
    const left = (canvasRect.left - wrapRect.left) + hoverX;
    const top = (canvasRect.top - wrapRect.top) + PAD.top;
    tooltipEl.style.left = `${left}px`;
    tooltipEl.style.top = `${top}px`;
  }

  let legendItems = [];   // stable DOM refs, built once - never recreated on poll

  function buildLegend() {
    legendEl.innerHTML = "";
    legendItems = seriesMeta.map((m, i) => {
      const item = document.createElement("label");
      item.className = "legend-item" + (hiddenSeries.has(i) ? " hidden" : "");

      const checkbox = document.createElement("input");
      checkbox.type = "checkbox";
      checkbox.checked = !hiddenSeries.has(i);
      checkbox.addEventListener("change", () => {
        if (checkbox.checked) hiddenSeries.delete(i);
        else hiddenSeries.add(i);
        item.classList.toggle("hidden", !checkbox.checked);
        if (!tableEl.hidden) renderTable();
        draw();
      });
      item.appendChild(checkbox);

      const swatch = document.createElement("span");
      swatch.className = "swatch";
      swatch.style.background = m.color;
      item.appendChild(swatch);

      const label = document.createElement("span");
      label.className = "legend-label";
      label.textContent = m.path;
      item.appendChild(label);

      const value = document.createElement("span");
      value.className = "legend-value";
      item.appendChild(value);

      legendEl.appendChild(item);
      return { item, valueEl: value };
    });
  }

  function updateLegendValues() {
    legendItems.forEach(({ item, valueEl }, i) => {
      const m = seriesMeta[i];
      const s = latest ? latest.series[i] : null;
      const hasData = s && s.t.length > 0;
      item.classList.toggle("waiting", !hasData);
      if (m.error) valueEl.textContent = "error";
      else if (!m.topic) valueEl.textContent = "waiting...";
      else if (hasData) valueEl.textContent = s.v[s.v.length - 1].toFixed(4);
      else valueEl.textContent = "no data";
    });
  }

  function renderTable() {
    tableBody.innerHTML = "";
    seriesMeta.forEach((m, i) => {
      const s = latest ? latest.series[i] : null;
      const hasData = s && s.t.length > 0;

      const tr = document.createElement("tr");

      const pathTd = document.createElement("td");
      pathTd.className = "mono";
      pathTd.textContent = m.path;
      tr.appendChild(pathTd);

      const topicTd = document.createElement("td");
      topicTd.className = "mono";
      topicTd.textContent = m.error ? m.error : (m.topic || "waiting...");
      tr.appendChild(topicTd);

      const valueTd = document.createElement("td");
      valueTd.className = "num";
      valueTd.textContent = hasData ? s.v[s.v.length - 1].toFixed(4) : "--";
      tr.appendChild(valueTd);

      tableBody.appendChild(tr);
    });
  }

  async function fetchSeries() {
    const res = await fetch("/api/series");
    seriesMeta = await res.json();
  }

  async function poll() {
    try {
      const res = await fetch("/api/data");
      latest = await res.json();
      updateLegendValues();
      if (!tableEl.hidden) renderTable();
      draw();
    } catch (err) {
      // Transient fetch failure (e.g. server restarting) - keep last frame.
    }
  }

  canvas.addEventListener("mousemove", (ev) => {
    const rect = canvas.getBoundingClientRect();
    hoverX = ev.clientX - rect.left;
    draw();
  });
  canvas.addEventListener("mouseleave", () => {
    hoverX = null;
    tooltipEl.hidden = true;
    draw();
  });

  tableToggle.addEventListener("change", () => {
    tableEl.hidden = !tableToggle.checked;
    canvas.parentElement.hidden = tableToggle.checked;
    if (tableToggle.checked) renderTable();
  });

  window.addEventListener("resize", resizeCanvas);

  (async () => {
    await fetchSeries();
    buildLegend();
    resizeCanvas();
    poll();
    setInterval(poll, 100);
  })();
})();
