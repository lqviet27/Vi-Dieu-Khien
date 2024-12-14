// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
// sensor
int vrx, vry;
const int SW_pin = 8;
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
    // Gạt trái => Thuận 90 độ
    BTSerial.write('0');
  } else if (vrx >= 800) {
  Serial.print("Phai\n");
    // Gạt phải => Ngược 90 độ
    BTSerial.write('1');
  }
  if (vry <= 200) {
  Serial.print("Len\n");
    // Gạt lên: Thuận 45 đọ
    BTSerial.write('2');
  } else if (vry >= 800) {
  Serial.print("Xuong\n");
    // Gạt Xuống : ngược 45 độ
    BTSerial.write('3');
  }

  if (SW_value == LOW) {
    Serial.println("Nhan nut");
    // Nhán : Thuận 180 
    BTSerial.write('4');
  }

  delay(2000);
}
