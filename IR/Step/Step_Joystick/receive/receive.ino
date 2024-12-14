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
const int speed = 15;  // [5, 15]
Stepper stepper(STEPS, IN1, IN3, IN2, IN4);


void setup() {
  Serial.begin(9600);
  stepper.setSpeed(speed);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode()) {
    NEC_received_command = irrecv.decodedIRData.command;
    Serial.println(NEC_received_command, HEX);
    if (NEC_received_command == 0x1A) {
      int angle = 90;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(step);
    } else if (NEC_received_command == 0x1B) {
      int angle = 90;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(-step);
    } else if (NEC_received_command == 0x1C) {
      // for (int i = 0; i < 5; i++) {
      //   stepper.step(0.1 * STEPS);
      //   delay(50);
      // }
      int angle = 30;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(step);
    } else if (NEC_received_command == 0x1D) {
      // for (int i = 0; i < 5; i++) {
      //   stepper.step(-0.1 * STEPS);
      //   delay(50);
      // }
      int angle = 30;
      int step = map(angle,0,360,0,STEPS);
      stepper.step(-step);
    } else if (NEC_received_command == 0x1E) {
      stepper.step(0);
      delay(100);
    }
    irrecv.resume();
  }
}
