#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

TTN_esp32 ttn;
TTN_CayenneLPP lpp;

uint32_t delayMS;

void setup() {
  Serial.begin(9600);

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }

  Serial.println(F("BME280 Sensor Example with TTN"));
  Serial.println(F("------------------------------------"));

  delayMS = 300000; // 5 minutes delay

  ttn.begin();
  ttn.onMessage([](const uint8_t *payload, size_t size, uint8_t port, int rssi) {
    Serial.println("-- MESSAGE");
    Serial.printf("Received %d bytes on port %d (RSSI=%ddB) :", size, port, rssi);
    for (int i = 0; i < size; i++) {
      Serial.printf(" %02X", payload[i]);
    }
    Serial.println();
  });

  ttn.join("70B3D57ED00626B9", "0000000000000000", "E26E0B46F262AB42A3D27EA8A5DDD102"); // Replace with your TTN credentials
}

void loop() {
  Serial.println("Taking BME280 reading...");

  Serial.print(F("Temperature: "));
  Serial.print(bme.readTemperature());
  Serial.println(F("°C"));

  Serial.print(F("Humidity: "));
  Serial.print(bme.readHumidity());
  Serial.println(F("%"));

  Serial.print(F("Pressure: "));
  Serial.print(bme.readPressure() / 100.0F); // hPa to Pa conversion
  Serial.println(F("hPa"));

  lpp.reset();
  lpp.addTemperature(1, bme.readTemperature());
  lpp.addRelativeHumidity(2, bme.readHumidity());
  lpp.addBarometricPressure(3, bme.readPressure() / 100.0F);

  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  Serial.println("Waiting for 5 minutes...");
  delay(delayMS);
}