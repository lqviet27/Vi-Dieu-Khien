// Master
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
// sensor
#include <DHT.h>
const int dht11_pin = 8;
const int dht_model = DHT11;
float humidity;
float temperature;
DHT dht11(dht11_pin, dht_model);
//
void setup() {
  BTSerial.begin(38400);
  Serial.begin(9600);
  dht11.begin();
}

void loop() {
  humidity = dht11.readHumidity();
  temperature = dht11.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Không có giá trị!");
  } else {
    Serial.print("Độ ẩm: ");
    Serial.print(humidity);

    Serial.print("%; Nhiêt độ: ");
    Serial.print(temperature);
    Serial.println("\u2103");
  }
  if (temperature <= 30) {
    BTSerial.write('0');
  } else {
    BTSerial.write('1');
  }
  delay(2000);
}
