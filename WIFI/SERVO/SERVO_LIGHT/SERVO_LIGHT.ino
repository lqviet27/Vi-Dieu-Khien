#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo myservo;
int angle = 0;
bool servoState = false;

// Thông tin Wi-Fi
const char* ssid = "BIETTO";
const char* password = "12345678";

// Chân kết nối servo và cảm biến ánh sáng
const int Servo_Pin = D2;
const int LightSensor_Pin = D3;

// Tạo đối tượng web server
ESP8266WebServer server(80);

// Biến thời gian để điều khiển servo không dùng delay
unsigned long previousMillis = 0;
const long interval = 500;

// Hàm trả về giao diện HTML
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>ESP8266 Light Sensor and Servo Control</title>";
  html += "<script>";

  // JavaScript để cập nhật giá trị ánh sáng
  html += "function updateLightData() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var response = JSON.parse(xhr.responseText);";
  html += "      document.getElementById('lightValue').innerHTML = response.value;";
  html += "      document.getElementById('lightStatus').innerHTML = response.status;";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/readLight', true);";
  html += "  xhr.send();";
  html += "}";

  // JavaScript để điều khiển servo
  html += "function controlServo() {";
  html += "  var angle = document.getElementById('angleInput').value;";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servoControl?angle=' + angle, true);";
  html += "  xhr.send();";
  html += "}";

  html += "setInterval(updateLightData, 1000);";  // Cập nhật mỗi 1 giây
  html += "</script></head><body>";

  html += "<h1>ESP8266 Light Sensor and Servo Control</h1>";

  // Hiển thị giá trị cảm biến ánh sáng và trạng thái
  html += "<p>Light Value: <span id='lightValue'>Loading...</span></p>";
  html += "<p>Light Status: <span id='lightStatus'>Loading...</span></p>";

  // Điều khiển servo
  html += "<input type='number' id='angleInput' placeholder='Enter angle (0-180)'>";
  html += "<button onclick='controlServo()'>Move Servo</button>";

  html += "</body></html>";
  return html;
}


// Hàm xử lý yêu cầu đọc giá trị cảm biến ánh sáng
void handleLightRead() {
  int lightValue = digitalRead(LightSensor_Pin);
  String lightStatus = (lightValue == 0) ? "Sáng" : "Tối";  // Điều chỉnh ngưỡng tùy theo môi trường
  String jsonResponse = "{\"value\": " + String(lightValue) + ", \"status\": \"" + lightStatus + "\"}";
  server.send(200, "application/json", jsonResponse);
}

// Hàm xử lý yêu cầu điều khiển servo
void handleServoControl() {
  if (server.hasArg("angle")) {
    int angle_ = server.arg("angle").toInt();
    if (angle_ >= 0 && angle_ <= 180) {
      angle = angle_;
      server.send(200, "text/plain", "Servo moved to " + String(angle) + " degrees");
    } else {
      server.send(400, "text/plain", "Invalid angle. Please enter a value between 0 and 180.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  myservo.attach(Servo_Pin, 500, 2400);
  myservo.write(0);

  // Định nghĩa các route của server
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateHTML());
  });
  server.on("/readLight", HTTP_GET, handleLightRead);
  server.on("/servoControl", HTTP_GET, handleServoControl);

  server.begin();
}

void loop() {
  server.handleClient();

  // Kiểm tra thời gian để điều khiển servo
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (servoState) {
      myservo.write(0);
    } else {
      myservo.write(angle);
    }
    servoState = !servoState;
  }
}
