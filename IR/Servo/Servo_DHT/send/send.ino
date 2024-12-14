#include <IRremote.h>

#include <DHT.h>
const int dht11_pin = 8;
const int dht_model = DHT11;
DHT dht11(dht11_pin, dht_model);


#define IR_SEND_PIN 7
IRsend irsend(IR_SEND_PIN);
uint32_t NEC_command;
uint32_t noRepeats = 2;

void setup() {
  dht11.begin();
  Serial.begin(9600);
}



float humidity;
float temperature;
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


    if (temperature >= 30 ) {
      NEC_command = 0x1C;
    } else {
      NEC_command = 0x1D;
    }

    irsend.sendNEC(0x00, NEC_command, noRepeats);
  }
  delay(500);
}