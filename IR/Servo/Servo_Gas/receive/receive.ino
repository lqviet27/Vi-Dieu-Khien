// #include "C:\\Users\\nguye\\OneDrive\\Documents\\Arduino\\libraries\\IRremote\\src\\IRremote.hpp"
#include <IRremote.h>
#include <Servo.h>

#define IR_RECEIVE_PIN 3
IRrecv irrecv(IR_RECEIVE_PIN);
uint32_t NEC_received_command;

Servo servo;

void setup() {
  servo.attach(10);
  servo.write(0);
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode()) {
    NEC_received_command = irrecv.decodedIRData.command;
    Serial.println(NEC_received_command);
    if (NEC_received_command == 0x1A) {
      int angle = 90;
      servo.write(angle);
      delay(500);
      servo.write(0);
      delay(500);
    }
    else if (NEC_received_command == 0x1B)  {
      int angle = 45;
      servo.write(angle);
      delay(500);
      servo.write(0);
      delay(500);
    }
    irrecv.resume();
  }
  delay(500);
}
