#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Servo.h>  // Thêm thư viện Servo

// Thông tin WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

// Khai báo server
ESP8266WebServer server(80);

// Khai báo chân điều khiển servo
#define SERVO_PIN D5  // Chân điều khiển servo 1

// Địa chỉ I2C của Arduino
#define SLAVE_ADDR 8  // D1-A5, D2-A4

// Khai báo servo
Servo myServo;  // Khai báo servo 1

// Biến lưu góc mục tiêu
int targetAngle = 0;      // Góc servo cần quay đến

// Biến thời gian cho millis()
unsigned long previousMillis = 0;
const long interval = 1000;  // 1 giây giữa các lần quay

// Trạng thái quay servo (true = quay đến targetAngle, false = quay về 0)
bool toTarget = true;


// Hàm gửi yêu cầu I2C và đọc dữ liệu từ Arduino
String readJoystickFromArduino() {
  Wire.requestFrom(SLAVE_ADDR, 100);  // Yêu cầu tối đa 20 byte từ Arduino
  String data = "";

  while (Wire.available()) {
    char c = Wire.read();
    data += c;
  }

  // Dữ liệu nhận được dạng "xValue,yValue,swPressed"
  return parseJoystickData(data);
}

// Hàm xử lý dữ liệu nhận từ Arduino
String parseJoystickData(String data) {
  int xValue, yValue;
  int swPressed;

  sscanf(data.c_str(), "%d,%d,%d", &xValue, &yValue, &swPressed);
  String swState = swPressed ? "Nhan" : "Khong nhan";

  return "X: " + String(xValue) + " | Y: " + String(yValue) + " | SW: " + swState;
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

  // Khởi tạo I2C
  Wire.begin();

  // Khởi tạo các servo
  myServo.attach(SERVO_PIN, 500, 2400);  // Gắn servo 1 vào chân D6
  myServo.write(0);

  // Cấu hình endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setServo", HTTP_GET, handleSetServo);
  server.on("/joystick", HTTP_GET, handleReadJoystick);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
   unsigned long currentMillis = millis();

  // Kiểm tra nếu đã đến thời điểm quay servo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Cập nhật thời gian

    if (toTarget) {
      myServo.write(targetAngle);  // Quay đến góc mục tiêu
      // Serial.print("Servo quay đến góc: ");
      // Serial.println(targetAngle);
    } else {
      myServo.write(0);  // Quay về 0
      // Serial.println("Servo quay về vị trí 0");
    }

    toTarget = !toTarget;  // Đảo trạng thái quay
  }
}


// Hàm xử lý trang web chính
void handleRoot() {
  String html = "<html><head>";
  html += "<script>";
  html += "function updateJoystick() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      document.getElementById('joystickValue').innerHTML = this.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/joystick', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "setInterval(updateJoystick, 1000);";

  html += "function sendServoCommand() {";
  html += "  var angle = document.getElementById('servoAngle').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/setServo?angle=' + angle, true);";
  html += "  xhttp.send();";
  html += "}";

  html += "</script>";
  html += "</head>";

  html += "<body>";
  html += "<h1>Joystick Values</h1>";
  html += "<p><span id='joystickValue'>X: 0 | Y: 0 | SW: Not Pressed</span></p>";

  html += "<h1>Servo Control</h1>";
  html += "<label for='servoAngle'>Nhap goc (0-180):</label>";
  html += "<input type='number' id='servoAngle' min='0' max='180' />";
  html += "<button onclick='sendServoCommand()'>Send Command</button>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Hàm đọc dữ liệu joystick từ Arduino
void handleReadJoystick() {
  String joystickData = readJoystickFromArduino();
  Serial.println(joystickData);
  server.send(200, "text/plain", joystickData);
}

// Hàm xử lý lệnh điều khiển servo
void handleSetServo() {
  String angleStr = server.arg("angle");
  int angle = angleStr.toInt();

  if (angle >= 0 && angle <= 180) {
    targetAngle = angle;        // Cập nhật góc mới
    toTarget = true;            // Đặt trạng thái quay đến góc mới
    previousMillis = millis();  // Reset thời gian để quay ngay lập tức
    server.send(200, "text/plain", "Servo quay đến góc: " + String(angle));
    Serial.println("Nhận lệnh quay đến góc: " + String(angle));
  } else {
    server.send(400, "text/plain", "Góc không hợp lệ. Vui lòng nhập góc từ 0 đến 180.");
  }
}
