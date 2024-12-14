// #include <SoftwareSerial.h>
// #include <Servo.h>
// Servo myServo;
// SoftwareSerial BTSerial(10, 11);

// void setup() {
//   myServo.attach(9);
//   Serial.begin(9600);
//   BTSerial.begin(38400);
// }

// void loop() {
//   if (BTSerial.available()) {
//     char receivedChar = BTSerial.read();
//     Serial.write(receivedChar);

//     switch (receivedChar) {
//       case '0':
//         myServo.write(30);
//         delay(500);
//         myServo.write(0);
//         delay(500);
//         break;
//       case '1':
//         myServo.write(60);
//         delay(500);
//         myServo.write(0);
//         delay(500);
//         break;
//       case '2':
//         myServo.write(120);
//         delay(500);
//         myServo.write(0);
//         delay(500);
//         break;
//       case '3':
//         myServo.write(150);
//         delay(500);
//         myServo.write(0);
//         delay(500);
//         break;
//       case '4':
//         myServo.write(180);
//         delay(500);
//         myServo.write(0);
//         delay(500);
//         break;
//       // Add more cases if needed
//       default:
//         // Handle other cases if neede
//         myServo.write(0);
//         delay(15);
//         break;
//     }
//     Serial.println();
//     // delay(500);
//   }
// }

#include <SoftwareSerial.h>
#include <Servo.h>

Servo myServo;
SoftwareSerial BTSerial(10, 11);

void setup() {
  myServo.attach(9, 500, 2400);  // Điều chỉnh các tham số của servo (min, max)
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop() {
  if (BTSerial.available()) {
    char receivedChar = BTSerial.read();
    Serial.write(receivedChar);

    switch (receivedChar) {
      case '0':
        while (true) {
          myServo.write(30);  // Quay servo đến góc 30
          delay(500);  // Đợi một chút cho servo di chuyển
          myServo.write(0);  // Quay về góc 0
          delay(500);  // Đợi một chút để servo quay về
          if (BTSerial.available()) {
            break;  // Thoát khỏi vòng lặp nếu nhận được lệnh mới
          }
        }
        break;
        
      case '1':
        while (true) {
          myServo.write(60);  // Quay servo đến góc 60
          delay(500);  // Đợi một chút cho servo di chuyển
          myServo.write(0);  // Quay về góc 0
          delay(500);  // Đợi một chút để servo quay về
          if (BTSerial.available()) {
            break;  // Thoát khỏi vòng lặp nếu nhận được lệnh mới
          }
        }
        break;

      case '2':
        while (true) {
          myServo.write(120);  // Quay servo đến góc 120
          delay(500);  // Đợi một chút cho servo di chuyển
          myServo.write(0);  // Quay về góc 0
          delay(500);  // Đợi một chút để servo quay về
          if (BTSerial.available()) {
            break;  // Thoát khỏi vòng lặp nếu nhận được lệnh mới
          }
        }
        break;

      case '3':
        while (true) {
          myServo.write(150);  // Quay servo đến góc 150
          delay(500);  // Đợi một chút cho servo di chuyển
          myServo.write(0);  // Quay về góc 0
          delay(500);  // Đợi một chút để servo quay về
          if (BTSerial.available()) {
            break;  // Thoát khỏi vòng lặp nếu nhận được lệnh mới
          }
        }
        break;

      case '4':
        while (true) {
          myServo.write(180);  // Quay servo đến góc 180
          delay(500);  // Đợi một chút cho servo di chuyển
          myServo.write(0);  // Quay về góc 0
          delay(500);  // Đợi một chút để servo quay về
          if (BTSerial.available()) {
            break;  // Thoát khỏi vòng lặp nếu nhận được lệnh mới
          }
        }
        break;

      // Thêm các trường hợp khác nếu cần
      default:
        myServo.write(0);  // Đặt servo về góc 0 khi không có lệnh
        delay(15);  // Đảm bảo servo có thời gian để di chuyển
        break;
    }
    Serial.println();
  }
}
