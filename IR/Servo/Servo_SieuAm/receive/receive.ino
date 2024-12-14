#include <IRremote.h>
#include <Servo.h>

#define IR_RECEIVE_PIN 8
const int SERVO_PIN = 3;

IRrecv irrecv(IR_RECEIVE_PIN);
uint32_t NEC_received_command;

Servo servo;

void setup() {
  servo.attach(SERVO_PIN);
  
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode()) {
    NEC_received_command = irrecv.decodedIRData.command;
    Serial.println(NEC_received_command);
    if (NEC_received_command == 0x1A) {
      int angle = 120;
      servo.write(angle);
      delay(1000);
      servo.write(0);
      delay(1000);
    }
    else if (NEC_received_command == 0x1B)  {
      int angle = 60;
      servo.write(angle);
      delay(1000);
      servo.write(0);
      delay(1000);
    }
    irrecv.resume();
  }
  delay(1000);
}
