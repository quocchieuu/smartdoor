#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "LED";
const char* password = "12345678";

WebServer server(80);

int ledState = 0; // 0: OFF, 1: ON

void handleRoot() {
  String onButtonColor = (ledState == 1) ? "background-color: #4CAF50;" : "background-color: #ccc;"; // Green or Gray
  String offButtonColor = (ledState == 0) ? "background-color: #f44336;" : "background-color: #ccc;"; // Red or Gray

  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
    <title>ESP32C3 LED Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
        height: 100vh;
        margin: 0;
        font-family: Arial, sans-serif;
      }
      h1 {
        color: #333;
        margin-bottom: 50px;
      }
      .button-container {
        display: flex;
        gap: 20px;
      }
      .control-button {
        padding: 20px 40px;
        font-size: 24px;
        font-weight: bold;
        color: white;
        text-decoration: none;
        border-radius: 10px;
        border: none;
        cursor: pointer;
        transition: background-color 0.3s, transform 0.2s;
      }
      .control-button:hover {
        transform: scale(1.05);
      }
    </style>
    </head>
    <body>
      <h1>ESP32C3 LED Control</h1>
      <div class="button-container">
        <a href="/on" class="control-button" style=")rawliteral" + onButtonColor + R"rawliteral(">ON</a>
        <a href="/off" class="control-button" style=")rawliteral" + offButtonColor + R"rawliteral(">OFF</a>
      </div>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleOn() {
  ledState = 1;
  Serial1.println("ON");
  Serial.println("LED State: ON"); // Print state to Serial Monitor
  server.sendHeader("Location", "/");
  server.send(303); // Redirect back to root to update button colors
  Serial.println("Send ON to STM32");
}

void handleOff() {
  ledState = 0;
  Serial1.println("OFF");
  Serial.println("LED State: OFF"); // Print state to Serial Monitor
  server.sendHeader("Location", "/");
  server.send(303); // Redirect back to root to update button colors
  Serial.println("Send OFF to STM32");
}

void setup() {
  Serial.begin(115200);       // Debug USB
  Serial1.begin(115200, SERIAL_8N1, 20, 21); // UART1: TX=21, RX=20 (adjust based on your board)

  WiFi.softAP(ssid, password);
  Serial.println("AP Started!");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Webserver started");
}

void loop() {
  server.handleClient();
}