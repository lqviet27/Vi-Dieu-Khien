#include <Wire.h>


// D1-A5, D2-A4

#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define JOYSTICK_SW 7

int xValue = 0;
int yValue = 0;
bool swPressed = false;

void setup() {
  Wire.begin(8);                     // Địa chỉ I2C là 8
  Wire.onRequest(sendJoystickData);  // Gọi hàm khi nhận yêu cầu từ Master
  pinMode(JOYSTICK_SW, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  xValue = analogRead(JOYSTICK_X);
  yValue = analogRead(JOYSTICK_Y);
  swPressed = digitalRead(JOYSTICK_SW) == LOW; // nhấn là low
  delay(50);
}


// Hàm gửi dữ liệu joystick
void sendJoystickData() {
  // Chuẩn bị dữ liệu dưới dạng chuỗi
  String data = String(xValue) + "," + String(yValue) + "," + (swPressed ? "1" : "0");
  Serial.println(data);
  // Gửi dữ liệu qua I2C từng byte một
  for (int i = 0; i < data.length(); i++) {
    Wire.write(data[i]);
  }
}
