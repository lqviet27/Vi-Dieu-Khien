#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Thông tin WiFi
const char *ssid = "BIETTO";
const char *password = "12345678";

// Khai báo server
ESP8266WebServer server(80);

// Khai báo chân kết nối cảm biến siêu âm và động cơ
#define TRIG_PIN D1       // Chân Trig kết nối với cảm biến siêu âm
#define ECHO_PIN D2       // Chân Echo kết nối với cảm biến siêu âm
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


// Hàm tính khoảng cách từ cảm biến siêu âm
long getDistance() {
  // Gửi tín hiệu HIGH lên chân TRIG để kích hoạt cảm biến
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2); // Đảm bảo thời gian đủ ngắn
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10); // Gửi tín hiệu trong 10 micro giây
  digitalWrite(TRIG_PIN, LOW);

  // Đo thời gian phản hồi từ Echo
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Tính toán khoảng cách (speed of sound = 343 m/s = 0.0343 cm/µs)
  long distance = (duration / 2) * 0.0343; // Đo khoảng cách theo cm

  return distance;
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

  // Cấu hình chân cảm biến siêu âm
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

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
  html += "      document.getElementById('distance').innerHTML = data.distance;"; 
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
  html += "<h1>Ultrasonic Sensor Data</h1>";
  html += "<p>Distance: <span id='distance'>--</span> cm</p>";

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
  long distance = getDistance();  // Đo khoảng cách từ cảm biến siêu âm

  Serial.print("Distance: ");
  Serial.println(distance);

  // Trả về dữ liệu dưới dạng JSON
  String json = "{";
  json += "\"distance\": " + String(distance);
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
