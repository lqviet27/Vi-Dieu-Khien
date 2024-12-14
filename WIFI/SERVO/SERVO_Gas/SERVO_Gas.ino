#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo myservo;
int angle = 0;
bool servoState = false;

// Thông tin Wi-Fi
const char* ssid = "BIETTO";
const char* password = "12345678";

// Chân kết nối servo và cảm biến khí gas
const int Servo_Pin = D2;
const int GasSensor_Pin = A0;  // Cảm biến khí gas thường dùng chân analog

// Tạo đối tượng web server
ESP8266WebServer server(80);

// Biến thời gian để điều khiển servo không dùng delay
unsigned long previousMillis = 0;
const long interval = 500;

// Hàm trả về giao diện HTML
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>ESP8266 Gas Sensor and Servo Control</title>";
  html += "<script>";

  // JavaScript để cập nhật giá trị cảm biến khí gas
  html += "function updateGasData() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var response = JSON.parse(xhr.responseText);";
  html += "      document.getElementById('gasValue').innerHTML = response.value;";
  html += "      document.getElementById('gasStatus').innerHTML = response.status;";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/readGas', true);";
  html += "  xhr.send();";
  html += "}";

  // JavaScript để điều khiển servo
  html += "function controlServo() {";
  html += "  var angle = document.getElementById('angleInput').value;";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servoControl?angle=' + angle, true);";
  html += "  xhr.send();";
  html += "}";

  html += "setInterval(updateGasData, 1000);";  // Cập nhật mỗi 1 giây
  html += "</script></head><body>";

  html += "<h1>ESP8266 Gas Sensor and Servo Control</h1>";

  // Hiển thị giá trị cảm biến khí gas và trạng thái
  html += "<p>Gas Sensor Value: <span id='gasValue'>Loading...</span></p>";
  html += "<p>Gas Status: <span id='gasStatus'>Loading...</span></p>";

  // Điều khiển servo
  html += "<input type='number' id='angleInput' placeholder='Enter angle (0-180)'>";
  html += "<button onclick='controlServo()'>Move Servo</button>";

  html += "</body></html>";
  return html;
}

// Hàm xử lý yêu cầu đọc giá trị cảm biến khí gas
void handleGasRead() {
  int gasValue = analogRead(GasSensor_Pin);
  String gasStatus = (gasValue > 400) ? "Có Gas!" : "Không có Gas";  // Ngưỡng tùy chỉnh theo cảm biến
  String jsonResponse = "{\"value\": " + String(gasValue) + ", \"status\": \"" + gasStatus + "\"}";
  server.send(200, "application/json", jsonResponse);
}

// Hàm xử lý yêu cầu điều khiển servo
void handleServoControl() {
  if (server.hasArg("angle")) {
    int angle_ = server.arg("angle").toInt();
    if (angle_ >= 0 && angle_ <= 180) {
      angle = angle_;
      myservo.write(angle);
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
    Serial.println("Connecting... ");
    delay(500);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  myservo.attach(Servo_Pin, 500, 2400);
  myservo.write(0);

  pinMode(GasSensor_Pin, INPUT);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateHTML());
  });
  server.on("/readGas", HTTP_GET, handleGasRead);
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
