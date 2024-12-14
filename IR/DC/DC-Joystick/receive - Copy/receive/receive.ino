#include <IRremote.h>

#define IR_RECEIVE_PIN 3
IRrecv irrecv(IR_RECEIVE_PIN);
uint32_t NEC_received_command;

#define DIR_1A_PIN 6
#define DIR_2A_PIN 7
#define EN12_PIN 8

void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
  digitalWrite(DIR_1A_PIN, output_1);
  digitalWrite(DIR_2A_PIN, output_2);
  analogWrite(EN12_PIN, speed);
  delay(delay_ms);
}

void setup() {
  pinMode(EN12_PIN, OUTPUT);
  pinMode(DIR_1A_PIN, OUTPUT);
  pinMode(DIR_2A_PIN, OUTPUT);

  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode()) {
    NEC_received_command = irrecv.decodedIRData.command;
    Serial.println(NEC_received_command,HEX);
    if (NEC_received_command == 0x1A) {
      dc_motor(LOW, HIGH, 255, 0);
    } else if (NEC_received_command == 0x1B) {
      dc_motor(HIGH, LOW, 255, 0);
    } else if (NEC_received_command == 0x1C) {
      for (int speed = 0; speed <= 255; speed += 2) {
        dc_motor(HIGH, LOW, speed, 20);
      }
        dc_motor(HIGH, LOW, 255, 500);
    } else if (NEC_received_command == 0x1D) {
      for (int speed = 255; speed >= 0; speed -= 2) {
        dc_motor(HIGH, LOW, speed, 20);
      }
        dc_motor(LOW, LOW, 0, 500);
    } else if (NEC_received_command == 0x1E) {
      dc_motor(LOW, LOW, 0, 0);
    }
    irrecv.resume();
  }
  // dc_motor(HIGH, LOW, 255, 500);
}
/***
Muốn quay liên tục thì để dc_motor(LOW, LOW, 0, 500); vào cuối và
 để delay khoảng 500 trong các hàm dc_motor: dc_motor(LOW, HIGH, 255, 500);
-> khi chỉnh như vậy thì nhớ chỉnh delay bên send cho hợp lý (1000)
***/