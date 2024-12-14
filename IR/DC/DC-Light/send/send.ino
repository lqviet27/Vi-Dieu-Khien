#include <IRremote.h>

const int LIGHT_PIN = A0;
int LIGHT_value;

#define IR_SEND_PIN 3
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 1;

void setup() {
  Serial.begin(9600);
}
void loop() {
  LIGHT_value = analogRead(LIGHT_PIN);
  Serial.println(LIGHT_value);
  if (LIGHT_value >= 650) { // toi
    NEC_command = 0x1A;
  } else {  // sang
    NEC_command = 0x1B;
  }

  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(500);
}