// #include "C:\\Users\\nguye\\OneDrive\\Documents\\Arduino\\libraries\\IRremote\\src\\IRremote.hpp"
#include <IRremote.h>

#define IR_RECEIVE_PIN 3
IRrecv irrecv(IR_RECEIVE_PIN);
uint32_t NEC_received_command;

#include <Stepper.h>
#define STEPS 2048
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
const int speed = 15; // [5, 15]
Stepper stepper(STEPS, IN1, IN3, IN2, IN4);


void setup() {
  stepper.setSpeed(speed);
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode()) {
    NEC_received_command = irrecv.decodedIRData.command;
    Serial.println(NEC_received_command);
    if (NEC_received_command == 0x1C) {
      int angle = 90;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(step);
      delay(500);
    }
    else if (NEC_received_command == 0x1D)  {
      int angle = 90;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(-step);
      delay(500);
    }
    irrecv.resume();
  }
}
