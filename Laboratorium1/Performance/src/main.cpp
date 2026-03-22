#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const unsigned long READ_INTERVAL = 2000; 
const int ONE_WIRE_BUS = 4;
const String DEVICE_ID = "ESP_SENSOR_01";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;
int healthCounter = 0; 

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= READ_INTERVAL) {
    previousMillis = currentMillis;

    sensors.requestTemperatures(); 
    float tempC = sensors.getTempCByIndex(0);

    String status;
    String value;

    if (tempC == DEVICE_DISCONNECTED_C) {
      healthCounter++;               
      status = "SENSOR_ERR";         
      value = "NaN";                 
    } else {
      healthCounter = 0;            
      status = "OK";                
      value = String(tempC);         
    }

    Serial.print(currentMillis);
    Serial.print(", ");
    Serial.print(DEVICE_ID);
    Serial.print(", ");
    Serial.print(value);
    Serial.print(", ");
    Serial.print(status);

    if (healthCounter > 0) {
      Serial.print(", HC: ");
      Serial.print(healthCounter);
    }
    
    Serial.println(); 
  }
}
