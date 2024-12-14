#include <SoftwareSerial.h>
#include <Servo.h>
Servo myServo;
SoftwareSerial BTSerial(10, 11);

void setup() {
  myServo.attach(9);
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop() {
  if (BTSerial.available()) {
    char receivedChar = BTSerial.read();
    Serial.write(receivedChar);

    switch (receivedChar) {
      case '0':
        myServo.write(90);
        delay(500);
        myServo.write(0);
        delay(500);
        break;
      case '1':
        myServo.write(30);
        delay(500);
        myServo.write(0);
        delay(500);
        break;
      // Add more cases if needed
      default:
        // Handle other cases if neede
        myServo.write(0);
        delay(15);
        break;
    }
    Serial.println();
    // delay(100);
  }
}
  