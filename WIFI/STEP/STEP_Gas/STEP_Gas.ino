#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Stepper.h>

// Kết nối WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

ESP8266WebServer server(80);

// Khai báo chân cảm biến khí gas (MQ-2 hoặc MQ-7)
#define GAS_SENSOR_PIN A0  // Chân analog của ESP8266 kết nối với cảm biến khí gas

// Khai báo chân động cơ
#define MOTOR_PIN_1 D1
#define MOTOR_PIN_2 D2
#define MOTOR_PIN_3 D3
#define MOTOR_PIN_4 D4
const int STEPS = 2048;

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

  // Cấu hình chân động cơ
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);
  myStepper.setSpeed(15);

  // Cấu hình chân cảm biến khí gas
  pinMode(GAS_SENSOR_PIN, INPUT);

  // Cấu hình endpoint
  server.on("/", HTTP_GET, handleRoot);
  server.on("/gas", HTTP_GET, handleReadGasSensor);
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
  html += "function updateValue() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
 html += "      var response = JSON.parse(this.responseText);";
  html += "      document.getElementById('gasValue').innerHTML = response.value;";
  html += "      document.getElementById('gasStatus').innerHTML = response.status;";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/gas', true);";
  html += "  xhttp.send();";
  html += "}"; 
  html += "setInterval(updateValue, 1000);";

  html += "function sendMotorCommand() {";
  html += "  var degrees = document.getElementById('degrees').value;";
  html += "  var direction = document.getElementById('direction').value;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/setMotor?degrees=' + degrees + '&direction=' + direction, true);";
  html += "  xhttp.send();";
  html += "}"; 
  html += "</script>";
  html += "</head>";

  html += "<body>";
  html += "<h1>Gas Sensor Status</h1>";
  html += "<p>Gas Sensor Value: <span id='gasValue'>Loading...</span></p>";
  html += "<p>Gas Status: <span id='gasStatus'>Loading...</span></p>";

  html += "<h2>Motor Control</h2>";
  html += "<label for='degrees'>Degrees:</label>";
  html += "<input type='number' id='degrees' name='degrees'><br><br>";
  html += "<label for='direction'>Direction:</label>";
  html += "<select id='direction'>";
  html += "<option value='xuoi'>Xuoi</option>";
  html += "<option value='nguoc'>Nguoc</option>";
  html += "</select><br><br>";
  html += "<button onclick='sendMotorCommand()'>Send Command</button>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleReadGasSensor() {
  int gasValue = analogRead(GAS_SENSOR_PIN);
  String gasStatus = (gasValue > 400) ? "Có Gas!" : "Không có Gas";  // Ngưỡng tùy chỉnh theo cảm biến
  String jsonResponse = "{\"value\": " + String(gasValue) + ", \"status\": \"" + gasStatus + "\"}";
  server.send(200, "application/json", jsonResponse);
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
