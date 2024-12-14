#include <IRremote.h>

const int GAS_PIN = A0;
int GAS_value;

#define IR_SEND_PIN 8
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 1;

void setup() {
  Serial.begin(9600);
}
void loop() {
  GAS_value = analogRead(GAS_PIN);
  Serial.println(GAS_value);
  if (GAS_value >= 650) {
    NEC_command = 0x1A;
  } else {
    NEC_command = 0x1B;
  }
  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(2000);
}