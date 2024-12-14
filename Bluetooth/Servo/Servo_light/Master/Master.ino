// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
// sensor
int value;
//
void setup() {
  BTSerial.begin(38400);
  Serial.begin(9600);
}

void loop() {
  value = analogRead(A0);
  Serial.println(value);
  if (value <= 700) {
    BTSerial.write('0');
  } else {
    BTSerial.write('1');
  }
  delay(2000);
}
