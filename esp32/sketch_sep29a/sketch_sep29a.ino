#include <WiFi.h>
    #include <WebServer.h>
    #include <LittleFS.h>

  

    const char* ssid = "The Door";
    const char* password = "12345678";





    // ✅ Trạng thái cửa
    bool doorOpen = false;
    bool servoOpen = false; // trạng thái servo hiện tại


    // Web server
    WebServer server(80);

    // UART config (nối với STM32)
    #define TX_PIN 21   // ESP32 TX → STM32 RX
    #define RX_PIN 20   // ESP32 RX ← STM32 TX

    // Load file từ LittleFS
    String loadFile(String path) {
      File file = LittleFS.open(path, "r");
      if (!file) {
        Serial.println("Failed to open file: " + path);
        return "File not found!";
      }
      String content;
      while (file.available()) {
        content += char(file.read());
      }
      file.close();
      return content;
    }

    // Trang chủ
    void handleRoot() {
      String html = loadFile("/index.h");
      String doorState = doorOpen ? "OPEN" : "CLOSED";
      html.replace("{{STATE}}", doorState);
      server.send(200, "text/html", html);
    }

    // Gửi lệnh mở cửa tới STM32
    void handleOpen() {
      Serial1.println("ON"); 
      server.send(200,"text/plain","Relay ON");
    }

    // Lấy trạng thái cửa
    void handleStatus() {
      String json = "{\"open\":";
      json += (doorOpen ? "true" : "false");
      json += "}";
      server.send(200, "application/json", json);
    }

    // Servo mở
    void handleServoOpen() {
      Serial1.println("SERVO_OPEN");   // ✅ chỉ gửi "SERVO_OPEN\n"
      server.send(200, "text/plain", "Servo Open 180");
    }

    // Servo đóng
    void handleServoClose() {
      Serial1.println("SERVO_CLOSE");  // ✅ chỉ gửi "SERVO_CLOSE\n"
      server.send(200, "text/plain", "Servo Close 0");
    }


    // Trả file tĩnh (ảnh, css…)
    void handleStaticFile(String path, String contentType) {
      File file = LittleFS.open(path, "r");
      if (!file) {
        server.send(404, "text/plain", "File Not Found");
        return;
      }
      server.streamFile(file, contentType);
      file.close();
    }

    void setup() {
      Serial.begin(115200);
      delay(2000); 
      

      if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed!");
      }

              // In trạng thái số lượng client kết nối
      WiFi.softAP(ssid, password);
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
      // ==== Router Web ====
      server.on("/", handleRoot);
      server.on("/open", handleOpen);
      server.on("/status", handleStatus);
      server.on("/on_servo", handleServoOpen);
      server.on("/off_servo", handleServoClose);

      // ảnh/logo
      server.on("/BG.png", []() { handleStaticFile("/BG.png", "image/png"); });
      server.on("/logo-iuh.png", []() { handleStaticFile("/logo-iuh.png", "image/png"); });

      server.begin();
      Serial.println("Web server started!");

      Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
      Serial.println("Serial1 started");


    }

    void loop() {
      server.handleClient();

      if (Serial1.available()) {
        String cmd = Serial1.readStringUntil('\n');
        cmd.trim();

        if (cmd == "BTN_OPEN") {
          Serial.println("STM32 báo cửa đã mở");
          doorOpen = true;
        }
        else if (cmd == "DOOR_CLOSED") {
          Serial.println("STM32 báo cửa đã đóng");
          doorOpen = false;
        }
        else if (cmd == "ACK_SERVO_OPEN") {
          servoOpen = true;
          Serial.println("[STM32] Xác nhận: Servo đã mở (180 độ)");
        }
        else if (cmd == "ACK_SERVO_CLOSE") {
        servoOpen = false;
        Serial.println("[STM32] Xác nhận: Servo đã đóng (0 độ)");
        }
        else if (cmd.startsWith("STM32:")) {
          // 🔹 In log STM32 gửi về
          Serial.println("Log từ STM32 → " + cmd);
        }
      }
    }