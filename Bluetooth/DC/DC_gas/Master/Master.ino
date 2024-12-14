// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
// sensor
int value;
#define A0 0
//
void setup() {
  BTSerial.begin(38400);
  Serial.begin(9600);
}

void loop() {
  value = analogRead(A0);
  Serial.println(value);
  if (value <= 600) {
    BTSerial.write('0');
  } else {
    BTSerial.write('1');
  }
  delay(1000);
}
