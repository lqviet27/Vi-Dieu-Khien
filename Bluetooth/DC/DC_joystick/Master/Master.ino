// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
// sensor
int vrx, vry;

int SW_pin = 8; // Nút nhấn trên joystick
//
void setup() {
  pinMode(SW_pin, INPUT_PULLUP);
  BTSerial.begin(38400);
  Serial.begin(9600);
}

void loop() {
  vrx = analogRead(A0);
  vry = analogRead(A1);
  Serial.print("x: ");
  Serial.print(vrx);
  Serial.print(", y: ");
  Serial.print(vry);
  Serial.println();

  // Đọc giá trị từ nút nhấn trên joystick
  int SW_value = digitalRead(SW_pin);

  if (vrx <= 200) {
  Serial.print("Trai\n");
    // Gạt trái => Thuận chiều
    BTSerial.write('0');
  } else if (vrx >= 800) {
  Serial.print("Phai\n");
    // Gạt phải => ngược chiều
    BTSerial.write('1');
  }
  if (vry <= 200) {
  Serial.print("Len\n");
    // Gạt lên: Nhanh dần
    BTSerial.write('2');
  } else if (vry >= 800) {
  Serial.print("Xuong\n");
    // Gạt Xuống : chậm dần
    BTSerial.write('3');
  } 

  if (SW_value == LOW) {
    Serial.println("Nhan nut");
    BTSerial.write('4');
  }
  
  delay(1500);
}
