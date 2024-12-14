#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Thông tin WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

// Khai báo server
ESP8266WebServer server(80);

// Khai báo chân kết nối động cơ và joystick
#define MOTOR_PIN_1 D6    // Chân điều khiển động cơ 1
#define MOTOR_PIN_2 D5    // Chân điều khiển động cơ 2
#define JOYSTICK_X_PIN A0 // Chân analog của joystick (X)
#define JOYSTICK_SW_PIN D3

// Hàm đọc giá trị từ joystick và điều khiển động cơ
void readJoystickAndControlMotor() {
  int joystickValue = analogRead(JOYSTICK_X_PIN);  // Đọc giá trị từ joystick (trục X)

  // Map giá trị joystick từ 0-1023 thành 0-255 để điều khiển tốc độ động cơ
  int motorSpeed = map(joystickValue, 0, 1023, 0, 255);

  // Điều khiển động cơ: Nếu joystick lệch sang trái, quay ngược lại, nếu lệch phải thì quay xuôi
  if (joystickValue < 200) {
    // Quay ngược
    analogWrite(MOTOR_PIN_1, HIGH);
    analogWrite(MOTOR_PIN_2, LOW);
  } else if (joystickValue > 800){
    // Quay xuôi
    analogWrite(MOTOR_PIN_1, LOW);
    analogWrite(MOTOR_PIN_2, HIGH);
  }
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

  // Cấu hình chân điều khiển động cơ
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
  // readJoystickAndControlMotor();
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
  html += "      document.getElementById('joy_x').innerHTML = data.v_x;"; 
  html += "       document.getElementById('joy_sw').innerHTML = data.sw"; // Cập nhật giá trị sw
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
  html += "<h1>JoyStick Data</h1>";
  html += "<p>X_value: <span id='joy_x'>--</span> </p>";
  html += "<p>Switch State (sw): <span id='joy_sw'>--</span> </p> "; //

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

// Hàm đọc dữ liệu từ cảm biến siêu âm
void handleReadSensor() {
  int jx = analogRead(JOYSTICK_X_PIN);  // Đo khoảng cách từ cảm biến siêu âm
  int sw = digitalRead(JOYSTICK_SW_PIN);
  Serial.print("x: ");
  Serial.print(jx);
  Serial.print("    sw: ");
  Serial.println(sw); 

  // Trả về dữ liệu dưới dạng JSON
  String json = "{";
  json += "\"v_x\": " + String(jx);
  json += ", ";
  json += "\"sw\": " + String(sw);
  json += "}";

  server.send(200, "application/json", json);
}

// Hàm điều khiển động cơ
void handleSetMotor() {
  String command = server.arg("command");

  if (command == "nhanh") {
    for (int speed = 0; speed <= 255; speed += 5) {
      analogWrite(MOTOR_PIN_1, speed);
      analogWrite(MOTOR_PIN_2, 0);
      delay(20);  
    }
    server.send(200, "text/plain", "Nhanh Dan");
    Serial.println("Nhanh Dan");
  } else if (command == "cham") {
    for (int speed = 255; speed >= 0; speed -= 2) {
      analogWrite(MOTOR_PIN_1, speed);
      analogWrite(MOTOR_PIN_2, 0);
      delay(20);  
    }
    server.send(200, "text/plain", "Cham Dan");
    Serial.println("Cham Dan");
  } else if (command == "xuoi") {
    digitalWrite(MOTOR_PIN_1, HIGH);
    digitalWrite(MOTOR_PIN_2, LOW);
    server.send(200, "text/plain", "Quay Xuoi");
    Serial.println("Quay Xuoi");
  } else if (command == "nguoc") {
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, HIGH);
    server.send(200, "text/plain", "Quay Nguoc");
    Serial.println("Quay Nguoc");
  } else if (command == "stop") {
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, LOW);
    server.send(200, "text/plain", "Dung Dong Co");
    Serial.println("Dung Dong Co");
  } else {
    server.send(400, "text/plain", "Invalid Command");
  }
}
