#include <IRremote.h>

#define IR_SEND_PIN 8
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 1; // 0 là không gửi lặp lại ,1 là gửi lại thêm 1 lân (tổng 2 lần gửi)

int SW_pin = 5; // Nút nhấn trên joystick
const int VRX_pin = A0;
const int VRY_pin = A1;

void setup() {
  pinMode(SW_pin, INPUT_PULLUP);
  Serial.begin(9600);
}

int x_pos, y_pos;
void loop() {
  x_pos = analogRead(VRX_pin);
  y_pos = analogRead(VRY_pin);
  Serial.print("(");
  Serial.print(x_pos);
  Serial.print(", ");
  Serial.print(y_pos);
  Serial.println(")");

  int SW_value = digitalRead(SW_pin);

  if (x_pos <= 200) {
    Serial.println("Trai");
    NEC_command = 0x1A;
  } else if (x_pos >= 800) {
    Serial.println("Phai");
    NEC_command = 0x1B;
  } else if (y_pos <= 200) {
    Serial.println("Len");
    NEC_command = 0x1C;
  } else if (y_pos >= 800) {
    Serial.println("Xuong");
    NEC_command = 0x1D;
  }

  if (SW_value == LOW) {
    Serial.println("Nhan");
    NEC_command = 0x1E;
  }

  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(1500);
}