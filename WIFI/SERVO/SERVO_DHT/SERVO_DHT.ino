#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <DHT.h>  // Thêm thư viện DHT

Servo myservo;  // Tạo đối tượng servo
int angle = 0;
bool servoState = false;  // Trạng thái servo (bật/tắt)

// Thông tin Wi-Fi
const char* ssid = "BIETTO";
const char* password = "12345678";

// Khai báo chân kết nối DHT11 và Servo
#define DHTPin D3      // Chân kết nối DHT11
#define DHTType DHT11  // Loại cảm biến DHT11
const int Servo_Pin = D2;

// Tạo đối tượng DHT
DHT dht(DHTPin, DHTType);

// Tạo đối tượng web server
ESP8266WebServer server(80);

// Biến thời gian để điều khiển servo không dùng delay
unsigned long previousMillis = 0;
const long interval = 700;  // Khoảng thời gian (ms) giữa các lần di chuyển servo

// Hàm trả về giao diện HTML
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>ESP8266 DHT11 and Servo Control</title>";
  html += "<script>";

  // JavaScript để cập nhật giá trị nhiệt độ và độ ẩm
  html += "function updateSensorData() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var data = JSON.parse(xhr.responseText);";
  html += "      document.getElementById('temperature').innerHTML = data.temperature;";
  html += "      document.getElementById('humidity').innerHTML = data.humidity;";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/readSensor', true);";
  html += "  xhr.send();";
  html += "}";

  // JavaScript để điều khiển servo
  html += "function controlServo() {";
  html += "  var angle = document.getElementById('angleInput').value;";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servoControl?angle=' + angle, true);";
  html += "  xhr.send();";
  html += "}";

  html += "setInterval(updateSensorData, 1000);";  // Cập nhật mỗi 1 giây
  html += "</script></head><body>";

  html += "<h1>ESP8266 DHT11 and Servo Control</h1>";

  // Hiển thị giá trị nhiệt độ và độ ẩm
  html += "<p>Temperature: <span id='temperature'>Loading...</span> &deg;C</p>";
  html += "<p>Humidity: <span id='humidity'>Loading...</span> %</p>";

  // Điều khiển servo
  html += "<input type='number' id='angleInput' placeholder='Enter angle (0-180)'>";
  html += "<button onclick='controlServo()'>Move Servo</button>";

  html += "</body></html>";
  return html;
}

// Hàm xử lý yêu cầu đọc giá trị cảm biến DHT11
void handleSensorRead() {
  float temperature = dht.readTemperature();  // Đọc nhiệt độ
  float humidity = dht.readHumidity();        // Đọc độ ẩm

  // Kiểm tra lỗi khi đọc dữ liệu
  if (isnan(temperature) || isnan(humidity)) {
    server.send(500, "text/plain", "Failed to read from DHT sensor!");
    return;
  }

  // Trả về dữ liệu dưới dạng JSON
  String json = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + " }";
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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  myservo.attach(Servo_Pin, 500, 2400);  // Kết nối servo
  myservo.write(0);
  dht.begin();  // Khởi động cảm biến DHT11

  // Định nghĩa các route của server
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", generateHTML());
  });
  server.on("/readSensor", HTTP_GET, handleSensorRead);
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
      myservo.write(0);  // Quay về góc 0 độ
    } else {
      myservo.write(angle);  // Di chuyển đến góc đã thiết lập
    }
    servoState = !servoState;  // Đảo trạng thái servo
  }
}
