#include <IRremote.h>

#define IR_SEND_PIN 7
const int LINE_PIN = 9;

IRsend irsend(IR_SEND_PIN);

uint32_t NEC_command;
uint32_t noRepeats = 2;
int darkness;

void setup() {
    Serial.begin(9600);
}
void loop() {
  darkness = digitalRead(LINE_PIN);
  int value = analogRead(A0);
  Serial.print(value);
  Serial.print("  ||  ");
  Serial.println(darkness);
  if (darkness == LOW) { // Line Trắng(nơi phản xạ ánh sáng) cho giá trị 0 || LOW
     NEC_command = 0x1A;
  } else {            // Line Đen(nơi không phản xạ ánh sáng) cho giá trị 1 || HIGH
    NEC_command = 0x1B;
  }

  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(500);
}

