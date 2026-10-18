#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>


// Thay đổi SSID và Password theo ý muốn
const char* ssid = "The Doorman";
const char* password = "12345678";

// Định nghĩa chân LED (GPIO 8 là LED tích hợp trên ESP32-C3 Mini)
const int ledPin = 8;

// Tạo đối tượng webserver trên cổng 80
WebServer server(80);


// Hàm load file từ LittleFS
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


// Xử lý trang chủ
void handleRoot() {
  String html = loadFile("/index.h");
  html.replace("%LED_STATUS%", digitalRead(ledPin) ? "ON" : "OFF");
  server.send(200, "text/html", html);
}

// Bật LED
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  String html = loadFile("/index.h");
  html.replace("%LED_STATUS%", "ON");
  server.send(200, "text/html", html);
}

// Tắt LED
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  String html = loadFile("/index.h");
  html.replace("%LED_STATUS%", "OFF");
  server.send(200, "text/html", html);
}

// Xử lý trả file tĩnh (ảnh, css, js)
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
  delay(1000);

  // Khởi tạo LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Mount LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted successfully.");

  // Thiết lập Access Point
  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Routes
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  // Routes cho file ảnh/logo
  server.on("/BG.png", []() {
    handleStaticFile("/BG.png", "image/png");
  });
  server.on("/logo-iuh.png", []() {
    handleStaticFile("/logo-iuh.png", "image/png");
  });

  // Khởi động server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}