#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "BIETTO";
const char *password = "12345678";

ESP8266WebServer server(80);

// Khai báo chân cảm biến và động cơ
#define LINE_SENSOR D3
// Khai báo chân kết nối động cơ và joystick
#define MOTOR_PIN_1 D6  // Chân điều khiển động cơ 1
#define MOTOR_PIN_2 D5  // Chân điều khiển động cơ 2
#define ENA_PIN D4      // Chân điều khiển enable (PWM)

// Hàm điều khiển động cơ DC với ENA và tốc độ PWM
void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
  digitalWrite(MOTOR_PIN_1, output_1);
  digitalWrite(MOTOR_PIN_2, output_2);
  analogWrite(ENA_PIN, speed);
  delay(delay_ms);
}

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Cấu hình chân động cơ
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);

  // Cấu hình endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/lineValue", HTTP_GET, handleReadSensor);
  server.on("/setMotor", HTTP_GET, handleSetMotor);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  int lineValue = digitalRead(LINE_SENSOR);

  String html = "<html><head>";
  html += "<script>";
  html += "function updateValue() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      document.getElementById('lineValue').innerHTML = this.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/lineValue', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "setInterval(updateValue, 1000);";

  html += "function sendMotorCommand() {";
  html += "  var command = document.getElementById('motorControl').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/setMotor?command=' + command, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "</script>";
  html += "</head>";

  html += "<body>";
  html += "<h1>Line Sensor Value</h1>";
  html += "<p>Digital Value: <span id='lineValue'>" + String(lineValue) + "</span></p>";

  html += "<h2>Motor Control</h2>";
  html += "<select id='motorControl'>";
  html += "<option value='nhanh'>Nhanh dan</option>";
  html += "<option value='cham'>Cham dan</option>";
  html += "<option value='xuoi'>Quay xuoi</option>";
  html += "<option value='nguoc'>Quay nguoc</option>";
  html += "<option value='stop'>Dung</option>";
  html += "</select>";
  html += "<button onclick='sendMotorCommand()'>Send Command</button>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleReadSensor() {
  int lineValue = digitalRead(LINE_SENSOR);
  server.send(200, "text/plain", String(lineValue));
}

void handleSetMotor() {
  String command = server.arg("command");

  if (command == "nhanh") {
    for (int speed = 0; speed <= 255; speed += 5) {
      dc_motor(HIGH, LOW, speed, 20);
    }
    server.send(200, "text/plain", "Nhanh Dan");
    Serial.println("Nhanh Dan");
  } else if (command == "cham") {
    for (int speed = 255; speed >= 0; speed -= 2) {
      dc_motor(HIGH, LOW, speed, 20);
    }
    server.send(200, "text/plain", "Cham Dan");
    Serial.println("Cham Dan");
  } else if (command == "xuoi") {
    dc_motor(HIGH, LOW, 255, 100);
    server.send(200, "text/plain", "Quay Xuoi");
    Serial.println("Quay Xuoi");
  } else if (command == "nguoc") {
    dc_motor(LOW, HIGH, 255, 100);
    server.send(200, "text/plain", "Quay Nguoc");
    Serial.println("Quay Nguoc");
  } else if (command == "stop") {
    dc_motor(LOW, LOW, 0, 100);
    server.send(200, "text/plain", "Dung Dong Co");
    Serial.println("Dung Dong Co");
  } else {
    server.send(400, "text/plain", "Invalid Command");
  }
}

