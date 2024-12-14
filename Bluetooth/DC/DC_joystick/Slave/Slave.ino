// // Slave
// #include <SoftwareSerial.h>
// SoftwareSerial BTSerial(10, 11);
// int command;
// // DC
// #define EN12_PIN 9
// #define DIR_1A_PIN 6
// #define DIR_2A_PIN 7

// void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
//   digitalWrite(DIR_1A_PIN, output_1);
//   digitalWrite(DIR_2A_PIN, output_2);
//   analogWrite(EN12_PIN, speed);
//   delay(delay_ms);
// }

// void setup() {
//   //
//   BTSerial.begin(38400);
//   Serial.begin(9600);

//   pinMode(EN12_PIN, OUTPUT);
//   pinMode(DIR_1A_PIN, OUTPUT);
//   pinMode(DIR_2A_PIN, OUTPUT);
// }

// void loop() {
//   if (BTSerial.available()) {
//     command = BTSerial.read();
//     Serial.print("Received command: ");
//     Serial.println(command);
//     if (command == '0') {
//       dc_motor(HIGH, LOW, 255, 0);
//     } else if (command == '1') {
//       dc_motor(LOW, HIGH, 255, 0);
//     } else if (command == '2') {
//       dc_motor(LOW, HIGH, 0, 100);
//       dc_motor(LOW, HIGH, 55, 100);
//       dc_motor(LOW, HIGH, 100, 100);
//       dc_motor(LOW, HIGH, 155, 100);
//       dc_motor(LOW, HIGH, 255, 100);
//     } else if (command == '3') {
//       dc_motor(LOW, HIGH, 255, 100);
//       dc_motor(LOW, HIGH, 155, 100);
//       dc_motor(LOW, HIGH, 100, 100);
//       dc_motor(LOW, HIGH, 55, 100);
//       dc_motor(LOW, HIGH, 0, 100);
//     } else if (command == '4'){
//       dc_motor(LOW, HIGH, 255, 250);
//       dc_motor(HIGH, LOW, 255, 250);
//     }
//   }
//   dc_motor(LOW, LOW, 0, 1000);
// }


#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11);
int command;

// DC Motor Pins
#define EN12_PIN 9
#define DIR_1A_PIN 6
#define DIR_2A_PIN 7

void dc_motor(int output_1, int output_2, int speed, int delay_ms) {
  digitalWrite(DIR_1A_PIN, output_1);
  digitalWrite(DIR_2A_PIN, output_2);
  analogWrite(EN12_PIN, speed);
  delay(delay_ms);
}

void setup() {
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
    
    if (command == '0') {  // Chạy liên tục xuôi
      while (true) {
        dc_motor(HIGH, LOW, 255, 10);  // chạy với tốc độ tối đa
        if (BTSerial.available()) break; // Thoát nếu nhận được lệnh mới
      }
    } 
    else if (command == '1') {  // Chạy liên tục ngược
      while (true) {
        dc_motor(LOW, HIGH, 255, 10);  // chạy ngược với tốc độ tối đa
        if (BTSerial.available()) break; // Thoát nếu nhận được lệnh mới
      }
    } 
    else if (command == '2') {  // Tăng tốc dần
      for (int speed = 0; speed <= 255; speed++) {
        dc_motor(HIGH, LOW, speed, 20);  // Tăng tốc dần
        if (BTSerial.available()) break; // Thoát nếu nhận được lệnh mới
      }
    } 
    else if (command == '3') {  // Giảm tốc dần
      for (int speed = 255; speed >= 0; speed--) {
        dc_motor(HIGH, LOW, speed, 20);  // Giảm tốc dần
        if (BTSerial.available()) break; // Thoát nếu nhận được lệnh mới
      }
    } 
    else if (command == '4') {  // Lệnh tắt động cơ
      dc_motor(LOW, LOW, 0, 0);  // Tắt động cơ
    }
  }
}
