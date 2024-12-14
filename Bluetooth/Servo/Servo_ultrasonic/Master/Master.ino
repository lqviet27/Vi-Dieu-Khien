// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11);
// sensor
const int trigger_pin = 5;
const int pw_pin = 6;
const int trigger_delay = 10;
long duration;
float cm;
//
void setup() {
  BTSerial.begin(38400);
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
  cm = duration * 0.034/2;
  Serial.print(cm);
  Serial.println(" cm");
  if (cm <= 10) {
    BTSerial.write('0');
  } else {
    BTSerial.write('1');
  }
  delay(2100);
}