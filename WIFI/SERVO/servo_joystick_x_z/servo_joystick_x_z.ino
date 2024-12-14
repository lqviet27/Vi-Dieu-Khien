#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo myservo;
int angle = 0;
bool servoState = false;  // Trạng thái servo (bật/tắt)

const char* ssid = "BIETTO";
const char* password = "12345678";

// Khai báo chân kết nối Joystick và Servo
#define JOYSTICK_X A0  // Chân analog X của joystick
#define BUTTON_PIN D4  // Chân nút bấm của joystick
const int Servo_Pin = D3;  // Chân Servo

// Tạo đối tượng web server
ESP8266WebServer server(80);

// Biến thời gian để điều khiển servo không dùng delay
unsigned long previousMillis = 0;
const long interval = 500;  // Khoảng thời gian (ms) giữa các lần di chuyển servo

// Hàm trả về giao diện HTML
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>ESP8266 Joystick and Servo Control</title>";
  html += "<script>";

  // JavaScript để cập nhật giá trị joystick và nút bấm
  html += "function updateJoystickData() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var data = JSON.parse(xhr.responseText);";
  html += "      document.getElementById('xValue').innerHTML = data.x;";
  html += "      document.getElementById('buttonStatus').innerHTML = data.button ? 'Pressed' : 'Not Pressed';";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/readJoystick', true);";
  html += "  xhr.send();";
  html += "}";

  // JavaScript để điều khiển servo
  html += "function controlServo() {";
  html += "  var angle = document.getElementById('angleInput').value;";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servoControl?angle=' + angle, true);";
  html += "  xhr.send();";
  html += "}";

  html += "setInterval(updateJoystickData, 1000);";  // Cập nhật mỗi 1 giây
  html += "</script></head><body>";

  html += "<h1>ESP8266 Joystick and Servo Control</h1>";

  // Hiển thị giá trị joystick
  html += "<p>X Value: <span id='xValue'>Loading...</span></p>";
  html += "<p>Button Status: <span id='buttonStatus'>Loading...</span></p>";

  // Điều khiển servo
  html += "<input type='number' id='angleInput' placeholder='Enter angle (0-180)'>";
  html += "<button onclick='controlServo()'>Move Servo</button>";

  html += "</body></html>";
  return html;
}

// Hàm xử lý yêu cầu đọc giá trị joystick
void handleJoystickRead() {
  int xValue = analogRead(JOYSTICK_X);  // Đọc giá trị X từ joystick (0-1023)
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;  // Kiểm tra nút bấm (nút bấm nối với GND khi nhấn)

  // Trả về dữ liệu dưới dạng JSON
  String json = "{ \"x\": " + String(xValue) + ", \"button\": " + (buttonPressed ? "true" : "false") + " }";
  server.send(200, "application/json", json);
}

// Hàm xử lý yêu cầu điều khiển servo
void handleServoControl() {
  if (server.hasArg("angle")) {
    int angle_ = server.arg("angle").toInt();  // Đọc góc từ request
    if (angle_ >= 0 && angle_ <= 180) {        // Giới hạn góc từ 0 đến 180 độ
      angle = angle_;
      server.send(200, "text/plain", "Servo moved to " + String(angle) + " degrees");
    } else {
      server.send(400, "text/plain", "Invalid angle. Please enter a value between 0 and 180.");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Đang kết nối WiFi...");
    delay(500);
  }
  Serial.println(WiFi.localIP());

  // Khởi động servo
  myservo.attach(Servo_Pin, 500, 2400);

  // Đặt chế độ cho các chân
  pinMode(JOYSTICK_X, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Nút bấm sẽ sử dụng pull-up, khi nhấn sẽ có trạng thái LOW

  // Định nghĩa các route của server
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateHTML());
  });
  server.on("/readJoystick", HTTP_GET, handleJoystickRead);
  server.on("/servoControl", HTTP_GET, handleServoControl);

  server.begin();
}

void loop() {
  server.handleClient();  // Xử lý yêu cầu từ client

  // Kiểm tra thời gian để điều khiển servo
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (servoState) {
      myservo.write(0);       // Quay về góc 0 độ
    } else {
      myservo.write(angle);   // Di chuyển đến góc đã thiết lập
    }
    servoState = !servoState; // Đảo trạng thái servo
  }
}
