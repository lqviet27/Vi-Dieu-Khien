// Slave
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11);
char command;
// DC
#define EN12_PIN 9
#define DIR_1A_PIN 7
#define DIR_2A_PIN 8

void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
  digitalWrite(DIR_1A_PIN, output_1);
  digitalWrite(DIR_2A_PIN, output_2);
  analogWrite(EN12_PIN, speed);
  delay(delay_ms);
}

void setup() {
  //
  BTSerial.begin(38400);
  Serial.begin(9600);

  pinMode(EN12_PIN, OUTPUT);
  pinMode(DIR_1A_PIN, OUTPUT);
  pinMode(DIR_2A_PIN, OUTPUT);
}

void loop() {
  if (BTSerial.available()) {
    command = BTSerial.read();
    Serial.print("Received command: ");
    Serial.println(command);
    if (command == '0') {
      dc_motor(LOW, HIGH, 255, 500);
    } else if (command == '1') {
      dc_motor(HIGH, LOW, 255, 500);

    }
  }
  dc_motor(LOW, LOW, 0, 1000);
}