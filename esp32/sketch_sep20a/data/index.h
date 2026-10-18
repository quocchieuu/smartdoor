<!doctype html>
<html lang="vi">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>Điều khiển cửa tự động — Remote Control</title>
  <style>
    :root{
      --bg:#0f1724;
      --card:#0b1220;
      --accent:#7c5cff;
      --accent-2:#00d4ff;
      --good:#22c55e;
      --warn:#f59e0b;
      --bad:#ef4444;
      --glass: rgba(255,255,255,0.03);
      --radius:14px;
      --glass-2: rgba(255,255,255,0.02);
      color-scheme: dark;
      font-family: Inter, ui-sans-serif, system-ui, -apple-system, "Segoe UI", Roboto, "Helvetica Neue", Arial;
    }
    *{box-sizing:border-box}
    html,body{height:100%}
    body{
      margin:0;
      background: linear-gradient(rgba(0,0,0,0.5), rgba(0,0,0,0.5)),  /* lớp phủ mờ */
      url("BG.png") no-repeat center center fixed;

      background-size: cover;   /* giúp ảnh phủ toàn màn hình */
      display:flex;align-items:center;justify-content:center;padding:32px;
    }
    .wrap{width:100%;max-width:1100px;display:grid;grid-template-columns:420px 1fr;gap:28px}

    .card{background:linear-gradient(180deg, rgba(255,255,255,0.02), transparent);border-radius:var(--radius);padding:20px;box-shadow:0 8px 30px rgba(2,6,23,0.6);border:1px solid rgba(255,255,255,0.03)}

    header.app{
      grid-column:1/-1;margin-bottom:6px;display:flex;align-items:center;gap:16px
    }
    header.app h1{margin:0;font-size:20px;color: #e6eef8}
    header.app p{margin:0;color:rgba(230,238,248,0.6)}

     /* Logo cố định */
    .logo-fixed {
      position: fixed;
      top: 10px;
      left: 10px;
      height: 60px;
      border-radius: 12px;
      z-index: 9999;
      background: none;   /* tuỳ chọn */
      padding: 0;        /* tuỳ chọn */
      /* Hiệu ứng viền phát sáng trắng */
      filter: drop-shadow(0 0 4px white);
      
      box-shadow: none;
    }



    /* Left column */
    .panel-main{display:flex;flex-direction:column;gap:18px}
    .door-visual{height:240px;border-radius:12px;background:linear-gradient(180deg,var(--glass),transparent);display:flex;align-items:center;justify-content:center;position:relative;overflow:hidden}
    .door{width:220px;height:180px;background:linear-gradient(180deg,#f8fafc,#e6eef8);border-radius:6px;display:flex;align-items:center;justify-content:center;position:relative;transform-origin:center;transition:transform 850ms cubic-bezier(.2,.9,.25,1)}

    .door.open{transform:translateX(120px) rotateY(-10deg) scale(1.02)}
    .door.closed{transform:translateX(0) rotateY(0)}

    .door .slot{position:absolute;left:12px;width:8px;height:40%;background:linear-gradient(90deg,#cbd5e1,#94a3b8);border-radius:8px}

    .status-strip{position:absolute;top:12px;left:12px;padding:6px 10px;border-radius:999px;background:linear-gradient(90deg, rgba(0,0,0,0.35), rgba(255,255,255,0.02));display:flex;gap:8px;align-items:center}
    .dot{width:12px;height:12px;border-radius:50%;box-shadow:0 2px 6px rgba(2,6,23,0.6)}

    .controls{display:flex;gap:12px;flex-wrap:wrap}
    /* Nút chính mặc định */
    .big-btn {
      flex: 1;
      padding: 16px;
      border-radius: 12px;
      font-weight: 700;
      font-size: 16px;
      border: none;
      cursor: pointer;
      transition: transform .12s ease, filter .12s;
      color: white;
    }
    .big-btn:active {
      transform: translateY(2px);
      filter: brightness(.96);
    }

    /* Màu riêng */
    .btn-blue {
      background: linear-gradient(180deg, #2563eb, #3b82f6); /* xanh dương */
    }
    .btn-green {
      background: linear-gradient(180deg, #16a34a, #22c55e); /* xanh lá */
    }
    .btn-red {
      background: linear-gradient(180deg, #b91c1c, #ef4444); /* đỏ */
    }
    .btn-gray {
      background: linear-gradient(180deg, #374151, #6b7280); /* xám */
    }
    .btn-yellow {
      background: linear-gradient(180deg, #d97706, #f59e0b); /* vàng cam */
      color: #fff8e1; /* chữ sáng hơn */
    }


    .small-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
    label.switch{display:flex;align-items:center;gap:10px;background:linear-gradient(180deg,var(--glass),transparent);padding:10px;border-radius:10px}
    input[type=checkbox]{width:42px;height:26px;appearance:none;background:#1f2937;border-radius:999px;position:relative;outline:none;cursor:pointer}
    input[type=checkbox]:after{content:"";position:absolute;left:4px;top:4px;width:18px;height:18px;border-radius:50%;background:white;transition:all .18s}
    input[type=checkbox]:checked{background:linear-gradient(90deg,var(--accent),var(--accent-2))}
    input[type=checkbox]:checked:after{left:20px}

    .log{height:220px;overflow:auto;padding:12px;background:linear-gradient(180deg,rgba(255,255,255,0.01),transparent);border-radius:10px;font-family:monospace;font-size:13px;color:rgba(230,238,248,0.9)}

    /* Right column */
    .panel-side{display:flex;flex-direction:column;gap:18px}
    .meta-row{display:flex;align-items:center;justify-content:space-between;gap:12px}
    .pill{padding:8px 12px;border-radius:999px;background:linear-gradient(90deg, rgba(255,255,255,0.02), transparent);border:1px solid rgba(255,255,255,0.03)}

    .status-box{display:flex;flex-direction:column;gap:10px}
    .indicator{display:flex;align-items:center;gap:10px}
    .indicator strong{font-size:16px;color:#e6eef8}

    .slider{width:100%}

    footer.hint{grid-column:1/-1;color:rgba(230,238,248,0.6);font-size:13px;margin-top:6px}



    /* responsive */
    @media (max-width:900px){
      .wrap{grid-template-columns:1fr;}
      .door-visual{height:200px}
    }
  </style>
</head>
<body>
  <img src="logo-iuh.png" alt="IUH Logo" class="logo-fixed">

  
  <div class="wrap">
    <header class="app">
  


      <div style="display:flex;flex-direction:column;gap:2px">
        <h1 style="color:white; margin-left:10px;">Điều khiển cửa tự động</h1>
        <p>Giao diện điều khiển từ xa — mô phỏng hoạt động, trạng thái, nhật ký và các chế độ.</p>
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
        
        <button id="openBtn" class="big-btn btn-green">Mở cửa</button>
        
      </div>

      <div style="display:flex;gap:8px;margin-top:10px">
        <button id="lockBtn" class="big-btn btn-gray">Khoá</button>
        <button id="unlockBtn" class="big-btn btn-yellow">Mở khoá</button>
      </div>

      <div class="small-grid">
        <label class="switch">Tự động
          <input id="autoMode" type="checkbox" />
        </label>

        <label style="display:flex;flex-direction:column;gap:6px;background:linear-gradient(180deg,var(--glass),transparent);padding:10px;border-radius:10px">
          Auto-close (giây)
          <input id="autoClose" class="slider" type="range" min="3" max="60" value="10" />
        </label>
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
        <div class="indicator"><strong>Chế độ:</strong><div id="modeText" style="margin-left:6px;color:rgba(230,238,248,0.8)">Manual</div></div>
        <div class="indicator"><strong>Thời gian đóng tự động:</strong><div id="autoTime" style="margin-left:6px;color:rgba(230,238,248,0.8)">10s</div></div>
        <div class="indicator"><strong>Ping:</strong><div id="ping" style="margin-left:6px;color:rgba(230,238,248,0.8)">—</div></div>
      </div>

      <div>
        <strong>Nhập mật khẩu</strong>
        <div style="margin-top:10px;display:flex;flex-direction:column;gap:10px">
          <!-- Nhập mật khẩu để mở cửa -->
          <div>
            <input id="passwordInput" type="password" placeholder="Nhập mật khẩu" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white">
            <button id="togglePass" style="margin-top:6px;padding:6px 10px;border-radius:8px;background:#374151;color:white;border:none;cursor:pointer">👁 Hiện/Ẩn</button>
            <button id="checkPassBtn" style="margin-top:6px;padding:8px 12px;border-radius:8px;background:#16a34a;color:white;border:none;cursor:pointer">Xác nhận mở cửa</button>
          </div>

          <!-- Nút hiển thị phần đổi mật khẩu -->
          <button id="toggleChangePass" style="margin-top:10px;padding:8px 12px;border-radius:8px;background:#2563eb;color:white;border:none;cursor:pointer">
           Đổi mật khẩu
          </button>

          <!-- KV doi mat khau -->
          <div id="changePassArea" style="display:none;margin-top:10px">
            <input id="oldPass" type="password" placeholder="Mật khẩu cũ" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white;margin-bottom:6px">
            <input id="newPass" type="password" placeholder="Mật khẩu mới" style="padding:8px;width:100%;border-radius:8px;border:1px solid #444;background:#1f2937;color:white;margin-bottom:6px">
            <button id="setPassBtn" style="padding:8px 12px;border-radius:8px;background:#f59e0b;color:white;border:none;cursor:pointer">Đặt mật khẩu mới</button>
          </div>
        </div>
      </div>
    </aside>

    <footer class="hint">Mẹo: dùng nút Bật / Tắt để mô phỏng lệnh từ xa. Chọn chế độ tự động để cửa đóng sau vài giây.</footer>
  </div>

  <script>
    // State
    const state = {
      open:false,
      locked:false,
      auto:false,
      autoClose: parseInt(localStorage.getItem('autoClose')||10,10),
      battery: 100
    }

    // Elements
    const doorEl = document.getElementById('door');
    const statusDot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    
    const openBtn = document.getElementById('openBtn');
    
    const logEl = document.getElementById('log');
    const clearLog = document.getElementById('clearLog');
    const doorState = document.getElementById('doorState');
    const batteryEl = document.getElementById('battery');
    const autoMode = document.getElementById('autoMode');
    const autoClose = document.getElementById('autoClose');
    const autoTime = document.getElementById('autoTime');
    const modeText = document.getElementById('modeText');
    const pingEl = document.getElementById('ping');
    const connLabel = document.getElementById('connLabel');
    const lockBtn = document.getElementById('lockBtn');
    const unlockBtn = document.getElementById('unlockBtn');

    // Utils
    function addLog(text){
      const t = new Date().toLocaleString();
      logEl.innerHTML = `<div>[${t}] ${text}</div>` + logEl.innerHTML;
    }

    function updateUI(){
      doorEl.classList.toggle('open', state.open);
      doorEl.classList.toggle('closed', !state.open);
      doorState.textContent = state.locked ? 'Khoá' : (state.open? 'Mở' : 'Đóng');
      batteryEl.textContent = state.battery + '%';
      autoMode.checked = state.auto;
      autoClose.value = state.autoClose;
      autoTime.textContent = state.autoClose + 's';
      modeText.textContent = state.auto ? 'Auto' : 'Manual';

      // Status strip
      if(state.open && !state.locked){
        statusDot.style.background = 'var(--good)';
        statusText.textContent = 'Cửa đã mở';
      } else if(!state.open && state.locked){
        statusDot.style.background = 'var(--bad)';
        statusText.textContent = 'Cửa khoá';
      } else if(!state.open){
        statusDot.style.background = 'var(--warn)';
        statusText.textContent = 'Cửa đã  đóng';
      }

      // Save some settings
      localStorage.setItem('autoClose', state.autoClose);
    }

    // Simulate ping/battery drain
    let pingTimer = setInterval(()=>{
      const ping = Math.floor(Math.random()*80)+20; // 20-100ms
      pingEl.textContent = ping + ' ms';
      // small battery drain
      state.battery = Math.max(0, state.battery - Math.random()*0.3);
      state.battery = Math.round(state.battery*10)/10;
      batteryEl.textContent = state.battery + '%';
    },3000);

    // Simulate connection status
    function setConnected(yes){
      connLabel.textContent = yes ? 'Online' : 'Offline';
      connLabel.style.color = yes ? 'var(--accent-2)' : 'rgba(230,238,248,0.45)';
    }

    // Actions
    function doOpen(){
      if(state.locked){ addLog('Bỏ qua: cửa đang khoá.'); return; }
      if(state.open){ addLog('Bỏ qua: cửa đã mở.'); return; }
      addLog('Lệnh: Mở cửa được gửi.');
      simulateDeviceResponse(true);
    }
    function doClose(){
      if(state.open===false){ addLog('Bỏ qua: cửa đã đóng.'); return; }
      addLog('Lệnh: Đóng cửa được gửi.');
      simulateDeviceResponse(false);
    }

    function simulateDeviceResponse(opening){
      // Simulate network latency
      statusText.textContent = 'Đang thực hiện lệnh...';
      setConnected(true);
      const latency = Math.floor(Math.random()*800)+120;
      setTimeout(()=>{
        state.open = !!opening;
        addLog('Thiết bị: Hoàn thành: ' + (state.open ? 'Mở' : 'Đóng'));
        updateUI();

        // if auto mode and door opened, schedule auto-close
        if(state.auto && state.open){
          addLog(`Auto: chu kỳ đóng sau ${state.autoClose}s.`);
          setTimeout(()=>{
            if(state.open && state.auto){
              addLog('Auto: đóng cửa...');
              state.open = false; updateUI();
            }
          }, state.autoClose*1000);
        }
      }, latency);
    }

    
    openBtn.addEventListener('click', checkPassword);
   
    clearLog.addEventListener('click', ()=>{ logEl.innerHTML=''; addLog('Nhật ký đã được xóa.'); });

    autoMode.addEventListener('change', (e)=>{
      state.auto = e.target.checked; updateUI(); addLog('Chuyển chế độ: ' + (state.auto ? 'Auto' : 'Manual'));
    });

    autoClose.addEventListener('input', (e)=>{
      state.autoClose = parseInt(e.target.value,10);
      autoTime.textContent = state.autoClose + 's';
      localStorage.setItem('autoClose', state.autoClose);
    });

    lockBtn.addEventListener('click', ()=>{
      state.locked = true; updateUI(); addLog('Lệnh: Khoá cửa.');
    });
    unlockBtn.addEventListener('click', ()=>{
      state.locked = false; updateUI(); addLog('Lệnh: Mở khoá cửa.');
    });

    // initial
    setTimeout(()=>{ setConnected(true); addLog('Giao diện sẵn sàng.'); state.open=false; updateUI(); }, 500);


   // --- Password System ---
    let savedPassword = localStorage.getItem('doorPass') || '1234'; // mặc định 1234

    const passInput = document.getElementById('passwordInput');
    const togglePass = document.getElementById('togglePass');
    const checkPassBtn = document.getElementById('checkPassBtn');
    const oldPass = document.getElementById('oldPass');
    const newPass = document.getElementById('newPass');
    const setPassBtn = document.getElementById('setPassBtn');
    const toggleChangePass = document.getElementById('toggleChangePass');
    const changePassArea = document.getElementById('changePassArea');
    

    // Ẩn/hiện mật khẩu
    togglePass.addEventListener('click', () => {
      passInput.type = passInput.type === 'password' ? 'text' : 'password';
  
    });

    // Kiểm tra mật khẩu để mở cửa
    function checkPassword() {
      if(passInput.value === savedPassword){
        addLog("Mật khẩu đúng → cho phép mở cửa.");
        doOpen(); // gọi mở cửa
      } else {
        addLog("Sai mật khẩu!");
        alert("Sai mật khẩu!");
      }
    }
    checkPassBtn.addEventListener('click', checkPassword);

    // Nhấn Enter ở ô nhập mật khẩu
    passInput.addEventListener('keypress', (e) => {
     if(e.key === 'Enter'){
      checkPassword();
    }


  });

    // Toggle hiển thị đổi mật khẩu
    toggleChangePass.addEventListener('click', () => {
      changePassArea.style.display = (changePassArea.style.display === 'none') ? 'block' : 'none';
    });

    // Đặt mật khẩu mới
    function setNewPassword(){
      if(oldPass.value !== savedPassword){
        alert("Mật khẩu cũ không đúng!");
        return;
      }
      if(newPass.value.trim() === ""){
        alert("Mật khẩu mới không được để trống!");
        return;
      }
      savedPassword = newPass.value;
      localStorage.setItem('doorPass', savedPassword);
      alert("Mật khẩu đã được thay đổi thành công!");
      addLog("Mật khẩu đã được thay đổi.");
      oldPass.value = "";
      newPass.value = "";
      changePassArea.style.display = 'none'; // ẩn lại sau khi đổi
    }
    setPassBtn.addEventListener('click', setNewPassword);

    // Nhấn Enter trong ô mật khẩu cũ / mới
    oldPass.addEventListener('keypress', (e) => {
      if(e.key === 'Enter') setNewPassword();
    });
    newPass.addEventListener('keypress', (e) => {
      if(e.key === 'Enter') setNewPassword();
    });

  </script>
</body>
</html>
