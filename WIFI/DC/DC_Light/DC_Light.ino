#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Thông tin WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

// Khai báo server
ESP8266WebServer server(80);

// Khai báo chân kết nối cảm biến và động cơ
#define LIGHT_SENSOR_PIN A0  // Chân kết nối cảm biến ánh sáng
#define MOTOR_PIN_1 D6       // Chân điều khiển động cơ 1
#define MOTOR_PIN_2 D5       // Chân điều khiển động cơ 2
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
  server.on("/sensorData", HTTP_GET, handleReadSensor);
  server.on("/setMotor", HTTP_GET, handleSetMotor);

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
  html += "function updateValue() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      var data = JSON.parse(this.responseText);";
  html += "      document.getElementById('lightLevel').innerHTML = data.light;"; 
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/sensorData', true);";
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
  html += "<h1>Light Sensor Data</h1>";
  html += "<p>Light Level: <span id='lightLevel'>--</span></p>";

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

// Hàm đọc dữ liệu từ cảm biến ánh sáng
void handleReadSensor() {
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);  // Đọc giá trị ánh sáng (0-1023)

  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  // Trả về dữ liệu dưới dạng JSON
  String json = "{";
  json += "\"light\": " + String(lightLevel);
  json += "}";

  server.send(200, "application/json", json);
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
