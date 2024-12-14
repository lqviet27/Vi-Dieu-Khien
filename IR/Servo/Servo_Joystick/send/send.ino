#include <IRremote.h>

#define IR_SEND_PIN 7
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 2;

const int VRX_pin = A0;
const int VRY_pin = A1;
int SW_pin = 4; // Nút nhấn trên joystick

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

  if (x_pos <= 300) {
    Serial.println("Trai");
    NEC_command = 0x1A;
  } else if (x_pos >= 700) {
    Serial.println("Phai");
    NEC_command = 0x1C;
  } else if (y_pos <= 300) {
    Serial.println("Len");
    NEC_command = 0x1B;
  } else if (y_pos >= 700) {
    Serial.println("Xuong");
    NEC_command = 0x1D;
  }

  if (SW_value == LOW) {
    Serial.println("CENTER");
    NEC_command = 0x1E;
  }
  
  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(500);
}