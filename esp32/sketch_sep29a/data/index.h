<!doctype html>
<html lang="vi">
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width,initial-scale=1" />
<title>Điều khiển cửa tự động — Remote Control</title>
<style>
  :root{
    --bg:#0f1724; --card:#0b1220; --accent:#7c5cff;
    --accent-2:#00d4ff; --good:#22c55e; --warn:#f59e0b;
    --bad:#ef4444; --glass: rgba(255,255,255,0.03);
    --radius:14px; --glass-2: rgba(255,255,255,0.02);
    color-scheme: dark;
    font-family: Inter, ui-sans-serif, system-ui, -apple-system, "Segoe UI", Roboto, "Helvetica Neue", Arial;
  }
  *{box-sizing:border-box}
  html,body{height:100%; margin:0; background: linear-gradient(rgba(0,0,0,0.5), rgba(0,0,0,0.5)), url("BG.png") no-repeat center center fixed; background-size: cover; display:flex;align-items:center;justify-content:center;padding:32px;}
  .wrap{width:100%;max-width:1100px;display:grid;grid-template-columns:420px 1fr;gap:28px}
  .card{background:linear-gradient(180deg, rgba(255,255,255,0.02), transparent);border-radius:var(--radius);padding:20px;box-shadow:0 8px 30px rgba(2,6,23,0.6);border:1px solid rgba(255,255,255,0.03)}
  header.app{grid-column:1/-1;margin-bottom:6px;display:flex;align-items:center;gap:16px}
  header.app h1{margin:0;font-size:20px;color: #e6eef8}
  header.app p{margin:0;color:rgba(230,238,248,0.6)}
  .logo-fixed {position: fixed; top: 10px; left: 10px; height: 60px; border-radius: 12px; z-index: 9999; filter: drop-shadow(0 0 4px white);}
  .panel-main{display:flex;flex-direction:column;gap:18px}
  .door-visual{height:240px;border-radius:12px;background:linear-gradient(180deg,var(--glass),transparent);display:flex;align-items:center;justify-content:center;position:relative;overflow:hidden}
  .door{width:220px;height:180px;background:linear-gradient(180deg,#f8fafc,#e6eef8);border-radius:6px;display:flex;align-items:center;justify-content:center;position:relative;transform-origin:center;transition:transform 850ms cubic-bezier(.2,.9,.25,1)}
  .door.open{transform:translateX(120px) rotateY(-10deg) scale(1.02)}
  .door.closed{transform:translateX(0) rotateY(0)}
  .door .slot{position:absolute;left:12px;width:8px;height:40%;background:linear-gradient(90deg,#cbd5e1,#94a3b8);border-radius:8px}
  .status-strip{position:absolute;top:3px;left:6px;padding:6px 10px;border-radius:999px;background:linear-gradient(90deg, rgba(0,0,0,0.35), rgba(255,255,255,0.02));display:flex;gap:8px;align-items:center}
  .dot{width:12px;height:12px;border-radius:50%;box-shadow:0 2px 6px rgba(2,6,23,0.6)}
  .controls{display:flex;gap:12px;flex-wrap:wrap}
  .big-btn {flex: 1; padding: 16px; border-radius: 12px; font-weight: 700; font-size: 16px; border: none; cursor: pointer; transition: transform .12s ease, filter .12s; color: white;}
  .big-btn:active {transform: translateY(2px); filter: brightness(.96);}
  .btn-blue {background: linear-gradient(180deg, #2563eb, #3b82f6);}
  .btn-green {background: linear-gradient(180deg, #16a34a, #22c55e);}
  .btn-red {background: linear-gradient(180deg, #b91c1c, #ef4444);}
  .btn-gray {background: linear-gradient(180deg, #374151, #6b7280);}
  .btn-yellow {background: linear-gradient(180deg, #d97706, #f59e0b); color: #fff8e1;}
  .small-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
  label.switch{display:flex;align-items:center;gap:10px;background:linear-gradient(180deg,var(--glass),transparent);padding:10px;border-radius:10px}
  input[type=checkbox]{width:42px;height:26px;appearance:none;background:#1f2937;border-radius:999px;position:relative;outline:none;cursor:pointer}
  input[type=checkbox]:after{content:"";position:absolute;left:4px;top:4px;width:18px;height:18px;border-radius:50%;background:white;transition:all .18s}
  input[type=checkbox]:checked{background:linear-gradient(90deg,var(--accent),var(--accent-2))}
  input[type=checkbox]:checked:after{left:20px}
  .log{height:220px;overflow:auto;padding:12px;background:linear-gradient(180deg,rgba(255,255,255,0.01),transparent);border-radius:10px;font-family:monospace;font-size:13px;color:rgba(230,238,248,0.9)}
  .panel-side{display:flex;flex-direction:column;gap:18px}
  .meta-row{display:flex;align-items:center;justify-content:space-between;gap:12px}
  .pill{padding:8px 12px;border-radius:999px;background:linear-gradient(90deg, rgba(255,255,255,0.02), transparent);border:1px solid rgba(255,255,255,0.03)}
  .status-box{display:flex;flex-direction:column;gap:10px}
  .indicator{display:flex;align-items:center;gap:10px}
  .indicator strong{font-size:16px;color:#e6eef8}
  .slider{width:100%}
  footer.hint{grid-column:1/-1;color:rgba(230,238,248,0.6);font-size:13px;margin-top:6px}
  @media (max-width:900px){.wrap{grid-template-columns:1fr;}.door-visual{height:200px}}
</style>
</head>
<body>
<img src="logo-iuh.png" alt="IUH Logo" class="logo-fixed">
<div class="wrap">
<header class="app">
  <div style="display:flex;flex-direction:column;gap:2px">
    <h1 style="color:white; margin-left:10px;">Điều khiển cửa tự động</h1>
    <p>Giao diện điều khiển từ xa — trạng thái, nhật ký và mật khẩu bảo mật.</p>
  </div>
  <div style="margin-left:auto;display:flex;gap:10px;align-items:center">
    <div class="pill">Kết nối: <strong id="connLabel">Online</strong></div>
    <div class="pill">Thiết bị: <strong>ESP-DOOR-01</strong></div>
  </div>
</header>

<section class="card panel-main">
  <div class="door-visual">
    <div class="status-strip">
      <div id="statusDot" class="dot" style="background:var(--warn)"></div>
      <div style="color:rgba(230,238,248,0.9)" id="statusText">Đang khởi động...</div>
    </div>
    <div id="door" class="door closed">
      <div class="slot"></div>
      <svg width="120" height="120" viewBox="0 0 24 24" style="opacity:.95">
        <path fill="#0b1220" d="M3 3h14v18H3z"/>
      </svg>
    </div>
  </div>

  <div class="controls">
    <button id="openBtn" class="big-btn btn-green">Mở khoá</button>
  </div>

  <div style="display:flex;gap:8px;margin-top:10px">
    <button id="lockBtn" class="big-btn btn-gray">Đóng cửa</button>
    <button id="unlockBtn" class="big-btn btn-yellow">Mở cửa</button>
  </div>

  <div>
    <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:6px">
      <strong>Nhật ký hoạt động</strong>
      <button id="clearLog" class="secondary" style="padding:6px 10px;border-radius:8px">Xóa</button>
    </div>
    <div id="log" class="log"></div>
  </div>
</section>

<aside class="card panel-side">
  <div class="meta-row">
    <div>
      <div style="color:rgba(230,238,248,0.9);font-weight:700">Trạng thái cửa</div>
      <div style="color:rgba(230,238,248,0.6)" id="doorState">Đang tải...</div>
    </div>
    <div class="pill">Pin: <strong id="battery">100%</strong></div>
  </div>

  <div class="status-box">
    <div class="indicator"><strong>Ping:</strong><div id="ping" style="margin-left:6px;color:rgba(230,238,248,0.8)">—</div></div>
  </div>

  <div>
    <strong>Nhập mật khẩu</strong>
    <div style="margin-top:10px;display:flex;flex-direction:column;gap:10px">
      <div>
        <input id="passwordInput" type="password" placeholder="Nhập mật khẩu" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white">
        <button id="togglePass" style="margin-top:6px;padding:6px 10px;border-radius:8px;background:#374151;color:white;border:none;cursor:pointer">👁 Hiện/Ẩn</button>
        <button id="checkPassBtn" style="margin-top:6px;padding:8px 12px;border-radius:8px;background:#16a34a;color:white;border:none;cursor:pointer">Xác nhận mở cửa</button>
      </div>

      <button id="toggleChangePass" style="margin-top:10px;padding:8px 12px;border-radius:8px;background:#2563eb;color:white;border:none;cursor:pointer">Đổi mật khẩu</button>

      <div id="changePassArea" style="display:none;margin-top:10px">
        <input id="oldPass" type="password" placeholder="Mật khẩu cũ" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white;margin-bottom:6px">
        <input id="newPass" type="password" placeholder="Mật khẩu mới" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white;margin-bottom:6px">
        <button id="setPassBtn" style="padding:8px 12px;border-radius:8px;background:#f59e0b;color:white;border:none;cursor:pointer">Đặt mật khẩu mới</button>
      </div>
    </div>
  </div>
</aside>

<div id="securityOverlay" style="
  position: fixed; inset: 0; background: rgba(0,0,0,0.85);
  display: flex; align-items: center; justify-content: center;
  z-index: 99999; flex-direction: column; color: white;
  font-family: Inter, sans-serif;">
  
  <div style="background: rgba(20,20,35,0.9); padding: 28px; border-radius: 14px; width: 320px; text-align: center; box-shadow: 0 8px 30px rgba(0,0,0,0.5)">
    <h2 style="margin-top:0;">🔐 Bảo mật truy cập</h2>
    <p style="font-size:14px;color:rgba(255,255,255,0.7)">Nhập mật khẩu để truy cập hệ thống</p>

    <!-- Ô nhập mật khẩu chính -->
    <div style="position:relative; margin-top:14px;">
      <input id="loginPass" type="password" placeholder="Mật khẩu"
        style="padding:10px 38px 10px 10px;width:100%;border-radius:8px;border:none;background:#1f2937;color:white;font-size:15px">
      <span class="eye" data-target="loginPass" style="position:absolute; right:10px; top:9px; cursor:pointer; font-size:18px;">👁</span>
    </div>

    <button id="loginBtn" style="margin-top:14px;padding:10px 16px;border-radius:8px;border:none;background:#16a34a;color:white;font-weight:600;cursor:pointer;width:100%">Đăng nhập</button>
    <button id="showChangeBtn" style="margin-top:10px;padding:8px 12px;border:none;border-radius:8px;background:#2563eb;color:white;cursor:pointer;width:100%">Đổi mật khẩu</button>
    
    <!-- Khu vực đổi mật khẩu -->
    <div id="changeArea" style="display:none;margin-top:10px;text-align:left">
      <div style="position:relative;margin-bottom:6px;">
        <input id="oldLoginPass" type="password" placeholder="Mật khẩu cũ" style="padding:8px 38px 8px 10px;width:100%;border-radius:8px;border:none;background:#1f2937;color:white;">
        <span class="eye" data-target="oldLoginPass" style="position:absolute; right:10px; top:7px; cursor:pointer;">👁</span>
      </div>
      <div style="position:relative;margin-bottom:6px;">
        <input id="newLoginPass" type="password" placeholder="Mật khẩu mới" style="padding:8px 38px 8px 10px;width:100%;border-radius:8px;border:none;background:#1f2937;color:white;">
        <span class="eye" data-target="newLoginPass" style="position:absolute; right:10px; top:7px; cursor:pointer;">👁</span>
      </div>
      <button id="saveLoginPass" style="padding:8px 12px;border:none;border-radius:8px;background:#f59e0b;color:white;cursor:pointer;width:100%">Lưu mật khẩu mới</button>
    </div>
  </div>
</div>

<footer class="hint">Mẹo: dùng nút "Mở cửa" hoặc mật khẩu để điều khiển.</footer>
</div>

<script>
const state = { open:false, locked:false, battery:100 };
let passwordCorrect = false;

const doorEl = document.getElementById('door');
const statusDot = document.getElementById('statusDot');
const statusText = document.getElementById('statusText');
const openBtn = document.getElementById('openBtn');
const logEl = document.getElementById('log');
const clearLog = document.getElementById('clearLog');
const doorState = document.getElementById('doorState');
const batteryEl = document.getElementById('battery');
const pingEl = document.getElementById('ping');
const connLabel = document.getElementById('connLabel');
const lockBtn = document.getElementById('lockBtn');
const unlockBtn = document.getElementById('unlockBtn');

function addLog(text){
  const t = new Date().toLocaleString();
  logEl.innerHTML = `<div>[${t}] ${text}</div>` + logEl.innerHTML;
}

function updateUI(){
  doorEl.classList.toggle('open', state.open);
  doorEl.classList.toggle('closed', !state.open);
  doorState.textContent = state.locked ? 'Khoá' : (state.open? 'Mở' : 'Đóng');
  batteryEl.textContent = state.battery + '%';

  openBtn.disabled = state.locked;
  lockBtn.disabled = state.locked;
  unlockBtn.disabled = state.locked;

  if(state.open && !state.locked){
    statusDot.style.background = 'var(--good)';
    statusText.textContent = 'Cửa đã mở';
  } else if(!state.open && state.locked){
    statusDot.style.background = 'var(--bad)';
    statusText.textContent = 'Cửa khoá';
  } else if(!state.open){
    statusDot.style.background = 'var(--warn)';
    statusText.textContent = 'Cửa đã đóng';
  }
}

setInterval(()=>{ 
  const ping = Math.floor(Math.random()*80)+20;
  pingEl.textContent = ping + ' ms';
  state.battery = Math.max(0, state.battery - Math.random()*0.3);
  state.battery = Math.round(state.battery*10)/10;
  batteryEl.textContent = state.battery + '%';
},3000);

function setConnected(yes){
  connLabel.textContent = yes ? 'Online' : 'Offline';
  connLabel.style.color = yes ? 'var(--accent-2)' : 'rgba(230,238,248,0.45)';
}

function sendCommand(cmd) {
  fetch('/' + cmd)
  .then(()=>addLog("Đã gửi lệnh: " + cmd.toUpperCase()))
  .catch(()=>addLog("Lỗi khi gửi lệnh: " + cmd));
}

function doOpen(){
  if(state.locked){ addLog('Bỏ qua: cửa đang khoá.'); alert('Hãy mở khoá trước!'); return;  }
  if(state.open){ addLog('Bỏ qua: cửa đã mở.'); return; }

  addLog('Lệnh: Mở cửa → gửi tới ESP32...');
  fetch('/open') // gửi request thật đến ESP32
    .then(res => res.text())
    .then(txt => {
      addLog("Phản hồi từ ESP32: " + txt);
      state.open = true;
      updateUI();
    })
    .catch(err => {
      addLog("❌ Lỗi gửi lệnh mở cửa: " + err);
    });
}


function simulateDeviceResponse(opening){
  statusText.textContent = 'Đang thực hiện lệnh...';
  setConnected(true);
  const latency = Math.floor(Math.random()*800)+120;
  setTimeout(()=>{
    state.open = !!opening;
    addLog('Thiết bị: Hoàn thành: ' + (state.open ? 'Mở' : 'Đóng'));
    updateUI();
  }, latency);
}

openBtn.addEventListener('click', checkPassword);
clearLog.addEventListener('click', ()=>{ logEl.innerHTML=''; addLog('Đã xóa nhật ký'); });

lockBtn.addEventListener('click', ()=>{
  if (state.locked) {
    alert('Hãy mở khoá trước!');
    return;
  }
  if (!state.open) {
    alert('Vui lòng mở khóa trước!');
    addLog('❗ Vui lòng mở cửa khóa khi thao tác.');
    return;
  }
  fetch('/off_servo')
    .then(()=>addLog("Servo đóng cửa"))
    .catch(()=>addLog("Lỗi gửi lệnh đóng servo"));
});

unlockBtn.addEventListener('click', ()=>{
  if (state.locked) {
    alert('Hãy mở khoá trước!');
    return;
  }
  if (!state.open) {
    alert('Vui lòng mở khóa trước!');
    addLog('❗ Vui lòng mở khóa trước khi thao tác.');
    return;
  }
  fetch('/on_servo')
    .then(()=>addLog("Servo mở cửa"))
    .catch(()=>addLog("Lỗi gửi lệnh mở servo"));
});


setTimeout(()=>{ setConnected(true); addLog('Giao diện sẵn sàng.'); updateUI(); }, 500);

// --- Password ---
let savedPassword = localStorage.getItem('doorPass') || '1234';
const passInput = document.getElementById('passwordInput');
const togglePass = document.getElementById('togglePass');
const checkPassBtn = document.getElementById('checkPassBtn');
const oldPass = document.getElementById('oldPass');
const newPass = document.getElementById('newPass');
const setPassBtn = document.getElementById('setPassBtn');
const toggleChangePass = document.getElementById('toggleChangePass');
const changePassArea = document.getElementById('changePassArea');

togglePass.addEventListener('click', () => {
  passInput.type = passInput.type === 'password' ? 'text' : 'password';
});

function unlockSystem() {
  state.locked = false;
  updateUI();
  addLog("✅ Hệ thống đã mở khoá. Có thể điều khiển cửa.");
  statusText.textContent = "Đã mở khoá - sẵn sàng điều khiển";
  unlockTimeout = setTimeout(() => {
    state.locked = true;
    updateUI();
    addLog("🔒 Hệ thống tự khoá lại sau 5 giây.");
    alert("Hệ thống đã tự khoá lại!");
  }, 5000);
}

function checkPassword() {
  if(passInput.value === savedPassword){
    passwordCorrect = true;
    addLog("Mật khẩu đúng → mở cửa.");
    doOpen(); 
  } else {
    passwordCorrect = false;
    addLog("Sai mật khẩu!");
    alert("Sai mật khẩu!");
  }
}
checkPassBtn.addEventListener('click', checkPassword);


 passInput.addEventListener('keypress', (e) => {
  if(e.key === 'Enter'){
  checkPassword();
  }

 });

toggleChangePass.addEventListener('click', () => {
  changePassArea.style.display = (changePassArea.style.display === 'none') ? 'block' : 'none';
});

function setNewPassword(){
  if(oldPass.value !== savedPassword){ alert("Mật khẩu cũ không đúng!"); return; }
  if(newPass.value.trim() === ""){ alert("Mật khẩu mới không được để trống!"); return; }
  savedPassword = newPass.value;
  localStorage.setItem('doorPass', savedPassword);
  alert("Mật khẩu đã được thay đổi!");
  addLog("Mật khẩu đã đổi.");
  oldPass.value = "";
  newPass.value = "";
  changePassArea.style.display = 'none';
}
setPassBtn.addEventListener('click', setNewPassword);

oldPass.addEventListener('keypress', (e) => {
      if(e.key === 'Enter') setNewPassword();
    });
    newPass.addEventListener('keypress', (e) => {
      if(e.key === 'Enter') setNewPassword();
    });

function pollStatus(){
  fetch('/status')
    .then(res => res.json())
    .then(data => {
      if (state.open !== data.open) {
        state.open = data.open;
        addLog("ESP32 báo: cửa " + (state.open ? "MỞ" : "ĐÓNG"));
        updateUI();
      }
      
    });
}
setInterval(pollStatus, 2000);

let sitePass = localStorage.getItem('sitePass') || '1234';
const overlay = document.getElementById('securityOverlay');
const loginPass = document.getElementById('loginPass');
const loginBtn = document.getElementById('loginBtn');
const showChangeBtn = document.getElementById('showChangeBtn');
const changeArea = document.getElementById('changeArea');
const oldLoginPass = document.getElementById('oldLoginPass');
const newLoginPass = document.getElementById('newLoginPass');
const saveLoginPass = document.getElementById('saveLoginPass');


document.body.style.overflow = 'hidden';

document.querySelectorAll('.eye').forEach(icon => {
  icon.addEventListener('click', () => {
    const target = document.getElementById(icon.dataset.target);
    target.type = target.type === 'password' ? 'text' : 'password';
  });
});

overlay.addEventListener('keydown', e => {
  if (e.key === 'Enter') {
    e.preventDefault();
    if (changeArea.style.display === 'block') {
      saveLoginPass.click();
    } else {
      loginBtn.click();
    }
  }
});

loginBtn.addEventListener('click', () => {
  if (loginPass.value === sitePass) {
    overlay.style.display = 'none';
    document.body.style.overflow = 'auto';
  } else {
    alert('Sai mật khẩu!');
  }
});


showChangeBtn.addEventListener('click', () => {
  changeArea.style.display = (changeArea.style.display === 'none') ? 'block' : 'none';
});

saveLoginPass.addEventListener('click', () => {
  if (oldLoginPass.value !== sitePass) {
    alert('Mật khẩu cũ không đúng!');
    return;
  }
  if (newLoginPass.value.trim() === '') {
    alert('Mật khẩu mới không được để trống!');
    return;
  }
  sitePass = newLoginPass.value;
  localStorage.setItem('sitePass', sitePass);
  alert('✅ Mật khẩu đã được đổi!');
  oldLoginPass.value = '';
  newLoginPass.value = '';
  changeArea.style.display = 'none';
});

document.body.style.overflow = 'hidden';

</script>
</body>
</html>
