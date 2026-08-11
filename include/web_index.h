#ifndef WEB_INDEX_H
#define WEB_INDEX_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32-S3 Barrier Control</title>
<style>
:root{--bg:#0f172a;--card:#1e293b;--border:#334155;--text:#f8fafc;--muted:#94a3b8;--green:#10b981;--red:#ef4444;--blue:#3b82f6;--amber:#f59e0b;--purple:#a855f7}
*{box-sizing:border-box;margin:0;padding:0;font-family:'Segoe UI',sans-serif}
body{background:var(--bg);color:var(--text);padding:12px;min-height:100vh}
.wrap{max-width:700px;margin:0 auto}

/* Header */
.hdr{text-align:center;padding:16px;background:var(--card);border-radius:12px;border:1px solid var(--border);margin-bottom:12px}
.hdr h1{font-size:1.3rem;color:#60a5fa;margin-bottom:3px}
.hdr p{color:var(--muted);font-size:.8rem}

/* Tab bar */
.tabs{display:flex;gap:4px;margin-bottom:12px}
.tab{flex:1;padding:10px 6px;border:1px solid var(--border);border-radius:8px;background:var(--card);color:var(--muted);font-size:.82rem;font-weight:bold;cursor:pointer;text-align:center;transition:all .2s}
.tab.active{background:#1d4ed8;color:#fff;border-color:#3b82f6}
.tab:hover:not(.active){background:#273549;color:var(--text)}

/* Tab content */
.pane{display:none}.pane.active{display:block}

/* Status bar */
.sbar{background:var(--card);padding:12px;border-radius:10px;border:1px solid var(--border);margin-bottom:12px;display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px}
.sbar-item{font-size:.82rem}.sbar-item label{color:var(--muted);margin-right:4px}
.badge{padding:3px 10px;border-radius:20px;font-size:.72rem;font-weight:bold;background:#475569;color:#fff}
.badge.ok{background:#059669}.badge.err{background:#dc2626}.badge.warn{background:#d97706}

/* === TAB 1: BARRIER CONTROL === */
.barrier-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-bottom:12px}
@media(max-width:600px){.barrier-grid{grid-template-columns:1fr}}

.barrier-card{background:var(--card);border-radius:12px;border:2px solid var(--border);padding:20px}
.barrier-title{display:flex;justify-content:space-between;align-items:center;margin-bottom:16px}
.barrier-title h2{font-size:1rem;color:#60a5fa}
.state-badge{padding:5px 14px;border-radius:20px;font-size:.8rem;font-weight:bold;transition:all .3s}
.state-IDLE{background:#334155;color:#94a3b8}
.state-UNKNOWN{background:#334155;color:#94a3b8}
.state-OPENING{background:#065f46;color:#6ee7b7;animation:pulse-glow 1s infinite}
.state-OPEN{background:#10b981;color:#fff}
.state-CLOSING{background:#7f1d1d;color:#fca5a5;animation:pulse-glow 1s infinite}
.state-CLOSED{background:#ef4444;color:#fff}
.state-STOPPING{background:#78350f;color:#fde68a;animation:pulse-glow 1s infinite}
.state-STOPPED{background:#f59e0b;color:#fff}
@keyframes pulse-glow{0%,100%{box-shadow:0 0 0 0 rgba(255,255,255,0);}50%{box-shadow:0 0 8px 3px rgba(255,255,255,.2)}}

.barrier-btns{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px}
.b-btn{padding:15px 4px;border:none;border-radius:10px;color:#fff;font-size:.85rem;font-weight:bold;cursor:pointer;transition:all .2s;display:flex;flex-direction:column;align-items:center;gap:5px;box-shadow:0 4px 12px rgba(0,0,0,.3)}
.b-btn:hover{transform:translateY(-2px);filter:brightness(1.15)}
.b-btn:active{transform:translateY(0);filter:brightness(.9)}
.b-btn .icon{font-size:1.3rem}
.b-btn.open {background:linear-gradient(135deg,#065f46,#10b981)}
.b-btn.stop {background:linear-gradient(135deg,#78350f,#f59e0b)}
.b-btn.close{background:linear-gradient(135deg,#7f1d1d,#ef4444)}
.b-btn:disabled{opacity:.4;cursor:not-allowed;transform:none}

/* Log box */
.log{background:#0a0f1e;border-radius:8px;border:1px solid var(--border);padding:10px;font-family:monospace;font-size:.75rem;max-height:150px;overflow-y:auto;margin-top:12px}
.log-entry{margin-bottom:3px;padding-bottom:2px;border-bottom:1px dashed #1e293b}
.log-entry.ok{color:#6ee7b7}.log-entry.err{color:#fca5a5}.log-entry.warn{color:#fde68a}.log-entry.info{color:#93c5fd}

/* === TAB 2: HARDWARE TEST === */
.hw-section{background:var(--card);border-radius:10px;border:1px solid var(--border);padding:14px;margin-bottom:12px}
.hw-section h3{font-size:.9rem;color:#60a5fa;margin-bottom:12px}
.ch-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:8px}
@media(max-width:480px){.ch-grid{grid-template-columns:repeat(2,1fr)}}
.ch-card{background:#0f172a;border-radius:8px;border:1px solid var(--border);padding:10px 6px;text-align:center}
.ch-card.active{border-color:var(--green);box-shadow:0 0 8px rgba(16,185,129,.3)}
.ch-dot{width:8px;height:8px;border-radius:50%;background:#475569;margin:0 auto 6px;transition:all .3s}
.ch-dot.on{background:var(--green);box-shadow:0 0 6px rgba(16,185,129,.7)}
.ch-label{font-size:.75rem;color:var(--muted);margin-bottom:6px}
.ch-label strong{color:var(--text)}
.ch-btns{display:flex;flex-direction:column;gap:3px}
.ch-btn{padding:4px;border:none;border-radius:5px;color:#fff;font-size:.7rem;font-weight:bold;cursor:pointer;transition:all .15s}
.ch-btn:hover{filter:brightness(1.2)}
.ch-btn.pulse{background:#1d4ed8}.ch-btn.on{background:#065f46}.ch-btn.off{background:#7f1d1d}
.hw-dur-row{display:flex;align-items:center;gap:8px;font-size:.8rem;color:var(--muted);margin-bottom:10px}
.hw-dur-row input[type=range]{flex:1;accent-color:#3b82f6}

.scan-result{background:#0f172a;border-radius:6px;border:1px solid var(--border);padding:10px;font-family:monospace;font-size:.78rem;color:#6ee7b7;margin-top:8px;min-height:50px;white-space:pre-wrap}
.scan-btn{padding:8px 16px;background:#1d4ed8;border:none;border-radius:6px;color:#fff;font-size:.82rem;font-weight:bold;cursor:pointer;margin-bottom:8px}
.scan-btn:hover{background:#2563eb}

/* === TAB 3: CONFIG === */
.cfg-card{background:var(--card);border-radius:10px;border:1px solid var(--border);padding:16px;margin-bottom:12px}
.cfg-card h3{font-size:.9rem;color:#60a5fa;margin-bottom:14px}
.form-row{margin-bottom:12px}
.form-row label{display:block;font-size:.78rem;color:var(--muted);margin-bottom:4px}
.form-row input{width:100%;padding:9px 12px;background:#0f172a;border:1px solid var(--border);border-radius:6px;color:var(--text);font-size:.85rem;outline:none;transition:border .2s;font-family:monospace;letter-spacing:.04em}
.form-row input:focus{border-color:#3b82f6}
.form-row input.valid  {border-color:#10b981!important;box-shadow:0 0 0 2px rgba(16,185,129,.15)}
.form-row input.invalid{border-color:#ef4444!important;box-shadow:0 0 0 2px rgba(239,68,68,.15)}
.field-err{display:none;font-size:.72rem;color:#f87171;margin-top:3px;padding-left:2px}
.field-err.show{display:block}
.save-btn{width:100%;padding:12px;background:linear-gradient(135deg,#1d4ed8,#3b82f6);border:none;border-radius:8px;color:#fff;font-size:.95rem;font-weight:bold;cursor:pointer;transition:all .2s}
.save-btn:hover:not(:disabled){filter:brightness(1.1);transform:translateY(-1px)}
.save-btn:disabled{opacity:.4;cursor:not-allowed;transform:none}
.redirect-box{display:none;background:#065f46;border-radius:8px;padding:12px;text-align:center;font-size:.9rem;color:#a7f3d0;margin-top:10px}

/* Disconnection Warning Banner */
.dis-banner{display:none;background:#dc2626;color:#fff;padding:12px 16px;border-radius:10px;text-align:center;font-size:.9rem;font-weight:bold;margin-bottom:12px;box-shadow:0 0 12px rgba(220,38,38,.5);animation:pulse-glow 1s infinite}

/* TCP status */
.tcp-clients{display:flex;align-items:center;gap:8px;font-size:.82rem;padding:10px;background:var(--card);border-radius:8px;border:1px solid var(--border)}
.tcp-dot{width:8px;height:8px;border-radius:50%;background:#475569;transition:all .3s}
.tcp-dot.active{background:var(--green);box-shadow:0 0 6px rgba(16,185,129,.7)}
</style>
</head>
<body>
<div class="wrap">

<div class="hdr">
  <h1>⚡ BARRIER CONTROL SYSTEM</h1>
  <p>ESP32-S3 | Dual Barrier | DI Feedback</p>
</div>

<!-- Status bar -->
<div class="sbar">
  <div class="sbar-item"><label>IP:</label><span id="ip-val">--</span></div>
  <div class="sbar-item"><label>Uptime:</label><span id="uptime-val">--</span></div>
  <div class="sbar-item"><span class="badge err" id="link-badge">Đang kết nối...</span></div>
  <div class="sbar-item tcp-clients">
    <div class="tcp-dot" id="tcp-dot"></div>
    <span id="tcp-count">0</span> TCP client
  </div>
</div>

<!-- Tab bar -->
<div class="tabs">
  <div class="tab active" id="tab-ctrl"   onclick="switchTab('ctrl')">🚧 Điều khiển</div>
  <div class="tab"        id="tab-hw"     onclick="switchTab('hw')">🔧 HW Test</div>
  <div class="tab"        id="tab-cfg"    onclick="switchTab('cfg')">⚙️ Cấu hình</div>
</div>

<!-- ===== TAB 1: BARRIER CONTROL ===== -->
<div class="pane active" id="pane-ctrl">
  <div class="dis-banner" id="dis-banner">⚠️ MẤT KẾT NỐI MẠNG — ĐÃ KHÓA TOÀN BỘ THAO TÁC</div>

  <div class="barrier-grid">
    <!-- BARRIER 1 -->
    <div class="barrier-card">
      <div class="barrier-title">
        <h2>🚧 BARRIER 1</h2>
        <span class="state-badge state-UNKNOWN" id="b1-state-badge">UNKNOWN</span>
      </div>

      <div class="barrier-btns">
        <button class="b-btn open"  id="b1-btn-open"  onclick="barrierCmd(1, 'open')">
          <span class="icon">🔓</span><span>MỞ</span>
        </button>
        <button class="b-btn stop"  id="b1-btn-stop"  onclick="barrierCmd(1, 'stop')">
          <span class="icon">✋</span><span>DỪNG</span>
        </button>
        <button class="b-btn close" id="b1-btn-close" onclick="barrierCmd(1, 'close')">
          <span class="icon">🔒</span><span>ĐÓNG</span>
        </button>
      </div>
    </div>

    <!-- BARRIER 2 -->
    <div class="barrier-card">
      <div class="barrier-title">
        <h2>🚧 BARRIER 2</h2>
        <span class="state-badge state-UNKNOWN" id="b2-state-badge">UNKNOWN</span>
      </div>

      <div class="barrier-btns">
        <button class="b-btn open"  id="b2-btn-open"  onclick="barrierCmd(2, 'open')">
          <span class="icon">🔓</span><span>MỞ</span>
        </button>
        <button class="b-btn stop"  id="b2-btn-stop"  onclick="barrierCmd(2, 'stop')">
          <span class="icon">✋</span><span>DỪNG</span>
        </button>
        <button class="b-btn close" id="b2-btn-close" onclick="barrierCmd(2, 'close')">
          <span class="icon">🔒</span><span>ĐÓNG</span>
        </button>
      </div>
    </div>
  </div>

  <div class="log" id="log-ctrl"><div class="log-entry info">[SYSTEM] Giao dien san sang.</div></div>
</div>

<!-- ===== TAB 2: HARDWARE TEST ===== -->
<div class="pane" id="pane-hw">
  <div class="hw-section">
    <h3>🔍 Quét I2C Bus</h3>
    <button class="scan-btn" onclick="doI2CScan()">▶ Bắt đầu quét</button>
    <div class="scan-result" id="scan-result">Bấm nút để bắt đầu quét...</div>
  </div>

  <div class="hw-section">
    <h3>🔧 Kiểm tra 8 kênh Relay</h3>
    <div class="hw-dur-row">
      <span>Thời gian xung test:</span>
      <input type="range" id="hw-dur" min="100" max="2000" step="100" value="400"
             oninput="document.getElementById('hw-dur-val').innerText=this.value+'ms'">
      <span style="color:#60a5fa;font-weight:bold;min-width:50px" id="hw-dur-val">400ms</span>
    </div>
    <div class="ch-grid" id="ch-grid"></div>
  </div>
</div>

<!-- ===== TAB 3: NETWORK CONFIG ===== -->
<div class="pane" id="pane-cfg">
  <div class="cfg-card">
    <h3>🌐 Cấu hình địa chỉ mạng</h3>
    <div class="form-row">
      <label>Địa chỉ IP (ESP32)</label>
      <input type="text" id="cfg-ip" placeholder="192.168.1.200" maxlength="15"
             oninput="filterIP(this)" onblur="validateIPField('cfg-ip','err-ip')">
      <div class="field-err" id="err-ip">⚠ Địa chỉ IP không hợp lệ (Giá trị từ 0-255)</div>
    </div>
    <div class="form-row">
      <label>Default Gateway</label>
      <input type="text" id="cfg-gw" placeholder="192.168.1.1" maxlength="15"
             oninput="filterIP(this)" onblur="validateIPField('cfg-gw','err-gw')">
      <div class="field-err" id="err-gw">⚠ Gateway không hợp lệ (VD: 192.168.1.1)</div>
    </div>
    <div class="form-row">
      <label>Subnet Mask</label>
      <input type="text" id="cfg-sn" placeholder="255.255.255.0" maxlength="15" value="255.255.255.0"
             oninput="filterIP(this)" onblur="validateIPField('cfg-sn','err-sn')">
      <div class="field-err" id="err-sn">⚠ Subnet Mask không hợp lệ (VD: 255.255.255.0)</div>
    </div>
    <button class="save-btn" id="save-cfg-btn" onclick="saveConfig()">💾 Lưu &amp; Khởi động lại</button>
    <div class="redirect-box" id="redirect-box">
      ✅ Đã lưu! Chuyển hướng tới <strong id="new-ip-txt"></strong> sau <span id="countdown">5</span>s...
    </div>
  </div>

  <div class="cfg-card">
    <h3>📡 TCP Push Server</h3>
    <p style="font-size:.82rem;color:var(--muted);margin-bottom:8px">
      Kết nối TCP đến <strong id="tcp-addr-show" style="color:#60a5fa">--</strong> để nhận bản tin sự kiện (JSON, mỗi dòng 1 sự kiện).
    </p>
    <p style="font-size:.78rem;color:var(--muted)">
      Sự kiện: <code style="color:#a5f3fc">barrier_cmd</code>, <code style="color:#a5f3fc">barrier_state</code>,
      <code style="color:#a5f3fc">relay_off</code>, <code style="color:#a5f3fc">barrier_rejected</code>
    </p>
  </div>
</div>

</div><!-- .wrap -->

<script>
// =================== TAB SWITCH ===================
function switchTab(name) {
  document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.pane').forEach(p => p.classList.remove('active'));
  document.getElementById('tab-'+name).classList.add('active');
  document.getElementById('pane-'+name).classList.add('active');
}

// =================== LOG ===================
function addLog(msg, type='info', boxId='log-ctrl') {
  const box = document.getElementById(boxId);
  const d = document.createElement('div');
  d.className = 'log-entry ' + type;
  d.innerText = '['+new Date().toLocaleTimeString()+'] '+msg;
  box.insertBefore(d, box.firstChild);
  while(box.children.length > 40) box.removeChild(box.lastChild);
}

let isRequestInFlight = false;
let networkLost = false;

function updateButtonStates(id, st) {
  const lockAll = isRequestInFlight || networkLost;
  const btnOpen  = document.getElementById('b'+id+'-btn-open');
  const btnStop  = document.getElementById('b'+id+'-btn-stop');
  const btnClose = document.getElementById('b'+id+'-btn-close');

  if (btnOpen)  btnOpen.disabled  = lockAll || (st === 'OPENING') || (st === 'OPEN');
  if (btnStop)  btnStop.disabled  = lockAll; // Stop can always be pressed unless network is lost
  if (btnClose) btnClose.disabled = lockAll || (st === 'CLOSING') || (st === 'CLOSED');
}

function handleNetworkLoss(msg) {
  if (!networkLost) {
    networkLost = true;
    document.getElementById('dis-banner').style.display = 'block';
    const badge = document.getElementById('link-badge');
    badge.innerText = 'OFFLINE';
    badge.className = 'badge err';
    addLog('⚠️ MẤT KẾT NỐI MẠNG: ' + (msg || 'Đã khóa toàn bộ thao tác'), 'err');
  }
  updateButtonStates(1, 'IDLE');
  updateButtonStates(2, 'IDLE');
}

// =================== STATUS UPDATE ===================
async function updateStatus() {
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), 1800);
  try {
    const r = await fetch('/api/status', { signal: controller.signal });
    clearTimeout(timeoutId);
    if (!r.ok) throw new Error('HTTP ' + r.status);
    const d = await r.json();

    if (!d.eth_link) {
      handleNetworkLoss('Đường truyền Ethernet bị đứt (Cable Link OFF)!');
      return;
    }

    // Kết nối bình thường -> Tắt banner cảnh báo nếu có
    if (networkLost) {
      networkLost = false;
      document.getElementById('dis-banner').style.display = 'none';
      addLog('✅ Đã khôi phục kết nối mạng!', 'ok');
    }

    document.getElementById('ip-val').innerText     = d.ip || '--';
    document.getElementById('uptime-val').innerText = d.uptime_s !== undefined ? Math.floor(d.uptime_s)+'s' : '--';
    const badge = document.getElementById('link-badge');
    badge.innerText = 'ETH OK'; badge.className = 'badge ok';

    // TCP clients
    const cnt = d.tcp_clients || 0;
    document.getElementById('tcp-count').innerText = cnt;
    document.getElementById('tcp-dot').className = 'tcp-dot' + (cnt>0?' active':'');

    // Barrier 1 state
    const st1 = d.barrier_1_state || 'UNKNOWN';
    const sb1 = document.getElementById('b1-state-badge');
    sb1.innerText = st1;
    sb1.className = 'state-badge state-'+st1;
    updateButtonStates(1, st1);

    // Barrier 2 state
    const st2 = d.barrier_2_state || 'UNKNOWN';
    const sb2 = document.getElementById('b2-state-badge');
    sb2.innerText = st2;
    sb2.className = 'state-badge state-'+st2;
    updateButtonStates(2, st2);

    // TCP address for config tab
    if (d.ip) {
      document.getElementById('tcp-addr-show').innerText = d.ip + ':8080';
    }

    // Relay states for hw tab
    const rb = d.relays_byte !== undefined ? d.relays_byte : 0;
    for (let i=1;i<=8;i++) {
      const dot  = document.getElementById('dot-ch'+i);
      const card = document.getElementById('card-ch'+i);
      if (!dot) continue;
      const on = (rb >> (i-1)) & 1;
      dot.className  = 'ch-dot' + (on?' on':'');
      card.className = 'ch-card' + (on?' active':'');
    }

    // Config tab: pre-fill IP, GW, Subnet fields if not edited yet
    const elIp = document.getElementById('cfg-ip');
    const elGw = document.getElementById('cfg-gw');
    const elSn = document.getElementById('cfg-sn');
    if (d.ip && !elIp.dataset.loaded)     { elIp.value = d.ip; elIp.dataset.loaded = '1'; validateIPField('cfg-ip','err-ip'); }
    if (d.gateway && !elGw.dataset.loaded){ elGw.value = d.gateway; elGw.dataset.loaded = '1'; validateIPField('cfg-gw','err-gw'); }
    if (d.subnet && !elSn.dataset.loaded) { elSn.value = d.subnet; elSn.dataset.loaded = '1'; validateIPField('cfg-sn','err-sn'); }
  } catch(e) {
    clearTimeout(timeoutId);
    handleNetworkLoss(e.name === 'AbortError' ? 'Timeout kết nối' : 'Không phản hồi');
  }
}

// =================== BARRIER COMMANDS ===================
async function barrierCmd(id, action) {
  if (networkLost || isRequestInFlight) return;

  isRequestInFlight = true;
  updateButtonStates(1, 'IDLE'); // Khóa tạm thời
  updateButtonStates(2, 'IDLE');

  addLog('Gửi lệnh: B' + id + ' ' + action.toUpperCase() + '...', 'info');
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), 2000);

  try {
    const r = await fetch('/api/barrier?id=' + id + '&action=' + action, { signal: controller.signal });
    clearTimeout(timeoutId);
    if (!r.ok) {
      const errJson = await r.json().catch(() => ({}));
      if (errJson.error === 'eth_link_off') {
        handleNetworkLoss('Dây mạng bị đứt (Link OFF)!');
        return;
      }
      throw new Error('HTTP ' + r.status);
    }
    const d = await r.json();
    if (d.result === 'ok') {
      addLog('Lệnh B' + id + ' ' + action.toUpperCase() + ' thành công', 'ok');
    }
  } catch(e) {
    clearTimeout(timeoutId);
    handleNetworkLoss(e.name === 'AbortError' ? 'Lệnh hết giờ (Timeout)' : e.message);
  } finally {
    isRequestInFlight = false;
    updateStatus();
  }
}

// =================== HARDWARE TAB ===================
// Build 8 channel cards
const grid = document.getElementById('ch-grid');
for (let i=1;i<=8;i++) {
  grid.innerHTML += `
  <div class="ch-card" id="card-ch${i}">
    <div class="ch-dot" id="dot-ch${i}"></div>
    <div class="ch-label"><strong>CH${i}</strong></div>
    <div class="ch-btns">
      <button class="ch-btn pulse" onclick="hwPulse(${i})">TEST</button>
      <button class="ch-btn on"    onclick="hwOn(${i})">BẬT</button>
      <button class="ch-btn off"   onclick="hwOff(${i})">TẮT</button>
    </div>
  </div>`;
}

async function hwCall(url) {
  try {
    const r = await fetch(url);
    await r.json();
    updateStatus();
  } catch(e) {}
}
function hwPulse(ch) { hwCall('/api/relay?ch='+ch+'&action=pulse&duration='+document.getElementById('hw-dur').value); }
function hwOn(ch)    { hwCall('/api/relay?ch='+ch+'&action=on'); }
function hwOff(ch)   { hwCall('/api/relay?ch='+ch+'&action=off'); }

async function doI2CScan() {
  document.getElementById('scan-result').innerText = 'Đang quét...';
  try {
    const r = await fetch('/api/i2cscan');
    const d = await r.json();
    if (d.results && d.results.length > 0) {
      let txt = '✅ Tìm thấy thiết bị:\n';
      d.results.forEach(r => {
        txt += `  SDA:${r.sda} SCL:${r.scl} → ${r.devices.join(', ')}\n`;
      });
      txt += `\nCấu hình hiện tại: SDA=${d.current_sda}, SCL=${d.current_scl}`;
      document.getElementById('scan-result').innerText = txt;
    } else {
      document.getElementById('scan-result').innerText = '❌ Không tìm thấy thiết bị I2C nào!\nKiểm tra lại dây nối SDA/SCL và nguồn cấp.';
    }
  } catch(e) { document.getElementById('scan-result').innerText = 'Lỗi: '+e.message; }
}

// =================== CONFIG TAB ===================

// Chỉ cho phép nhập số (0-9) và dấu chấm (.)
function filterIP(el) {
  const pos = el.selectionStart;
  const before = el.value;
  el.value = before.replace(/[^0-9.]/g, '');
  // Phục hồi vị trí con trỏ nếu có ký tự bị xóa
  if (el.value !== before) {
    el.selectionStart = el.selectionEnd = Math.max(0, pos - (before.length - el.value.length));
  }
  validateIPField(el.id, 'err-' + el.id.replace('cfg-',''));
}

// Kiểm tra định dạng IP hợp lệ: 4 octet, mỗi octet 0-255
function isValidIP(str) {
  if (!str) return false;
  const parts = str.split('.');
  if (parts.length !== 4) return false;
  return parts.every(p => {
    if (p === '' || p.length > 3) return false;
    const n = Number(p);
    return Number.isInteger(n) && n >= 0 && n <= 255;
  });
}

// Kiểm tra Subnet Mask hợp lệ (các bit 1 phải liên tục từ MSB)
function isValidSubnet(str) {
  if (!isValidIP(str)) return false;
  const parts = str.split('.').map(Number);
  const mask32 = ((parts[0] << 24) >>> 0) + (parts[1] << 16) + (parts[2] << 8) + parts[3];
  const inverted = (~mask32) >>> 0;
  return (inverted & (inverted + 1)) === 0;
}

// Validate 1 trường và hiện/ẩn thông báo lỗi
function validateIPField(inputId, errId) {
  const el  = document.getElementById(inputId);
  const err = document.getElementById(errId);
  if (!el || !err) return true;
  const ok = (inputId === 'cfg-sn') ? isValidSubnet(el.value.trim()) : isValidIP(el.value.trim());
  el.classList.toggle('valid',   ok);
  el.classList.toggle('invalid', !ok && el.value.trim() !== '');
  err.classList.toggle('show',   !ok && el.value.trim() !== '');
  updateSaveBtn();
  return ok;
}

// Cập nhật trạng thái nút Lưu (chỉ bật khi cả 3 trường hợp lệ)
function updateSaveBtn() {
  const btn = document.getElementById('save-cfg-btn');
  if (!btn) return;
  const allOk = isValidIP(document.getElementById('cfg-ip').value.trim())
             && isValidIP(document.getElementById('cfg-gw').value.trim())
             && isValidSubnet(document.getElementById('cfg-sn').value.trim());
  btn.disabled = !allOk;
}

async function saveConfig() {
  // Validate lại toàn bộ trước khi gửi
  const okIP = validateIPField('cfg-ip', 'err-ip');
  const okGW = validateIPField('cfg-gw', 'err-gw');
  const okSN = validateIPField('cfg-sn', 'err-sn');
  if (!okIP || !okGW || !okSN) {
    addLog('⚠ Vui lòng kiểm tra lại địa chỉ IP trước khi lưu!', 'warn');
    return;
  }

  const ip = document.getElementById('cfg-ip').value.trim();
  const gw = document.getElementById('cfg-gw').value.trim();
  const sn = document.getElementById('cfg-sn').value.trim();

  try {
    const r = await fetch('/api/config/setip?ip='+encodeURIComponent(ip)+'&gw='+encodeURIComponent(gw)+'&sn='+encodeURIComponent(sn));
    const d = await r.json();
    if (d.status === 'ok') {
      document.getElementById('new-ip-txt').innerText = ip;
      const box = document.getElementById('redirect-box');
      box.style.display = 'block';
      let cnt = 5;
      const timer = setInterval(() => {
        cnt--;
        document.getElementById('countdown').innerText = cnt;
        if (cnt <= 0) {
          clearInterval(timer);
          window.location.href = 'http://'+ip;
        }
      }, 1000);
    } else {
      alert('Lỗi: ' + (d.error || 'Không xác định'));
    }
  } catch(e) { alert('Không thể kết nối ESP32!'); }
}

// =================== INIT ===================
setInterval(updateStatus, 2000);
updateStatus();
</script>
</body>
</html>
)rawliteral";

#endif // WEB_INDEX_H
