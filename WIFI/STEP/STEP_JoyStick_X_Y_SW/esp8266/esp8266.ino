#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Stepper.h>
#include <Wire.h>

// Kết nối WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

ESP8266WebServer server(80);

// Khai báo chân động cơ Stepper
#define MOTOR_PIN_1 D5
#define MOTOR_PIN_2 D6
#define MOTOR_PIN_3 D7
#define MOTOR_PIN_4 D8
const int STEPS = 2048;

// Địa chỉ I2C của Arduino
#define SLAVE_ADDR 8

// Khởi tạo đối tượng Stepper
Stepper myStepper(STEPS, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);

// Biến lưu trữ trạng thái động cơ
int motorSteps = 0;
int motorDirection = 1;  // 1: xuôi, -1: ngược
bool motorRunning = false;
unsigned long previousMillis = 0;
long interval = 1000;
int stepsRemaining = 0;
int tmpSteps = 0;

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

  // Cấu hình chân động cơ
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);
  myStepper.setSpeed(15);

  // Cấu hình endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/joystick", HTTP_GET, handleReadJoystick);
  server.on("/setMotor", HTTP_GET, handleSetMotor);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (motorRunning) {
    if (stepsRemaining > 0) {
      myStepper.step(motorDirection);
      stepsRemaining--;
      previousMillis = millis();
    } else {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        stepsRemaining = tmpSteps;
      }
    }
  }
}

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
  html += "</script>";
  html += "</head>";

  html += "<body>";
  html += "<h1>Joystick Values</h1>";
  html += "<p><span id='joystickValue'>X: 0 | Y: 0 | SW: Not Pressed</span></p>";

  html += "<h2>Motor Control</h2>";
  html += "<label for='degrees'>Degrees:</label>";
  html += "<input type='number' id='degrees' name='degrees'><br><br>";
  html += "<label for='direction'>Direction:</label>";
  html += "<select id='direction'>";
  html += "<option value='xuoi'>Xuoi</option>";
  html += "<option value='nguoc'>Nguoc</option>";
  html += "</select><br><br>";
  html += "<button onclick='sendMotorCommand()'>Send Command</button>";

  html += "<script>";
  html += "function sendMotorCommand() {";
  html += "  var degrees = document.getElementById('degrees').value;";
  html += "  var direction = document.getElementById('direction').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/setMotor?degrees=' + degrees + '&direction=' + direction, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleReadJoystick() {
  String joystickData = readJoystickFromArduino();
  Serial.println(joystickData);
  server.send(200, "text/plain", joystickData);
}

// Hàm gửi yêu cầu I2C và đọc dữ liệu từ Arduino
String readJoystickFromArduino() {
  Wire.requestFrom(SLAVE_ADDR, 100); // Yêu cầu tối đa 100 byte từ Arduino
  String data = "";

  while (Wire.available()) {
    char c = Wire.read();
    data += c;
  }

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

void handleSetMotor() {
  String degreesStr = server.arg("degrees");
  String direction = server.arg("direction");

  int degrees = degreesStr.toInt();
  motorSteps = map(degrees, 0, 360, 0, STEPS);
  stepsRemaining = motorSteps;
  tmpSteps = motorSteps;

  if (direction == "xuoi") {
    motorDirection = 1;
    motorRunning = true;
    server.send(200, "text/plain", "Quay Xuôi " + degreesStr + " độ");
    Serial.println("Quay Xuôi " + degreesStr + " độ");
  } else if (direction == "nguoc") {
    motorDirection = -1;
    motorRunning = true;
    server.send(200, "text/plain", "Quay Ngược " + degreesStr + " độ");
    Serial.println("Quay Ngược " + degreesStr + " độ");
  } else {
    motorRunning = false;
    server.send(400, "text/plain", "Invalid Command");
  }
}
