#include <IRremote.h>

const int LINE_PIN = 4;

#define IR_SEND_PIN 8
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int line = digitalRead(LINE_PIN);
  Serial.println(line);
  if (line == LOW) { // trang
    NEC_command = 0x1A;
  } else { // Den
    NEC_command = 0x1B;
  }
  
  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(500);
}