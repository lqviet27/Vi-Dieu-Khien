#include <IRremote.h>
#define IR_SEND_PIN 8
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 2;



const int trigger_pin = 3;
const int pw_pin = 4;
const int trigger_delay = 10; 
double duration;                 
float cm;


void setup() {
  Serial.begin(9600);
  pinMode(trigger_pin, OUTPUT);
  pinMode(pw_pin, INPUT);
}
void loop() {
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(trigger_delay);
  digitalWrite(trigger_pin, LOW);
  duration = pulseIn(pw_pin, HIGH);
  cm = duration * 0.017;
  Serial.println(cm);
  if (cm <= 10) {
    NEC_command = 0x1C;
  } else {
    NEC_command = 0x1D;
  }
  
  irsend.sendNEC(0x00, NEC_command, noRepeats);
  delay(1000);
}