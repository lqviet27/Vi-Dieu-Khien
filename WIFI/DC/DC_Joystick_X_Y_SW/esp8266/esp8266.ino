#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

// Thông tin WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

// Khai báo server
ESP8266WebServer server(80);

// Khai báo chân kết nối động cơ và joystick
#define MOTOR_PIN_1 D6  // Chân điều khiển động cơ 1
#define MOTOR_PIN_2 D5  // Chân điều khiển động cơ 2
#define ENA_PIN D4      // Chân điều khiển enable (PWM)


// Địa chỉ I2C của Arduino
#define SLAVE_ADDR 8 // D1-A5, D2-A4

// Hàm điều khiển động cơ DC với ENA và tốc độ PWM
void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
  digitalWrite(MOTOR_PIN_1, output_1);
  digitalWrite(MOTOR_PIN_2, output_2);
  analogWrite(ENA_PIN, speed);
  delay(delay_ms);
}


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
  String swState = swPressed ? "Pressed" : "Not Pressed";

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

  // Cấu hình chân điều khiển động cơ
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  // Cấu hình endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setMotor", HTTP_GET, handleSetMotor);
  server.on("/joystick", HTTP_GET, handleReadJoystick);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
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

  html += "function sendMotorCommand() {";
  html += "  var command = document.getElementById('motorControl').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/setMotor?command=' + command, true);";
  html += "  xhttp.send();";
  html += "}";

  html += "</script>";
  html += "</head>";

  html += "<body>";
  html += "<h1>Joystick Values</h1>";
  html += "<p><span id='joystickValue'>X: 0 | Y: 0 | SW: Not Pressed</span></p>";

  html += "<h2>Motor Control</h2>";
  html += "<select id='motorControl'>";
  html += "<option value='nhanh'>Nhanh Dan</option>";
  html += "<option value='cham'>Cham Dan</option>";
  html += "<option value='xuoi'>Quay Xuoi</option>";
  html += "<option value='nguoc'>Quay Nguoc</option>";
  html += "<option value='stop'>Dung</option>";
  html += "</select>";
  html += "<button onclick='sendMotorCommand()'>Send Command</button>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Hàm đọc dữ liệu joystick từ Arduino
void handleReadJoystick() {
  String joystickData = readJoystickFromArduino();
  Serial.println(joystickData);
  server.send(200, "text/plain", joystickData);
}

// Hàm điều khiển động cơ
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
