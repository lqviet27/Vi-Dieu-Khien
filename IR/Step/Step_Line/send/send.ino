#include <IRremote.h>                                                          

const int LINE_PIN = 4;

#define IR_SEND_PIN 8
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 2;
int darkness;

void setup() {
    Serial.begin(9600);
}

void loop() {
  darkness = digitalRead(LINE_PIN);
  Serial.println(darkness);
  if (darkness == LOW) {
    NEC_command = 0x1C;
  } else {
    NEC_command = 0x1D;
  }
  
  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(1000);
}