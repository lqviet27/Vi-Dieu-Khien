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
    Serial.println(receivedChar);

    switch (receivedChar) {
      case '0':
        myServo.write(90);
        delay(1000);
        myServo.write(0);
        delay(1000);
        break;
      case '1':
        myServo.write(45);
        delay(1000);
        myServo.write(0);
        delay(1000);
        break;
      // Add more cases if needed
      default:
        // Handle other cases if neede
        myServo.write(0);
        delay(15);
        break;
    }
    delay(100);
  }
}
  