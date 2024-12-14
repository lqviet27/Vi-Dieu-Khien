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
  if (value <= 300) {
    BTSerial.write('1');
  } else {
    BTSerial.write('0');
  }
  delay(1500);
}
