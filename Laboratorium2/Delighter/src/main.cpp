#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h>        
#include <LiquidCrystal_I2C.h>  

ESP8266WebServer server(80);
Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

unsigned long previousMillis = 0;
const long interval = 2000; 
unsigned long lastRequestTime = 0;
unsigned int requestCount = 0;

void handleSensor() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastRequestTime < 2000) {
    server.send(429, "text/plain", "429 Too Many Requests");
    return; 
  }
  lastRequestTime = currentMillis;

  if (server.header("X-API-KEY") != "tajne-haslo-123") {
    server.send(401, "text/plain", "401 Unauthorized");
    return; 
  }

  Wire.beginTransmission(0x77);
  if (Wire.endTransmission() != 0) {
    server.send(503, "text/plain", "503 Service Unavailable");
    return; 
  }

  Serial.print("[HTTP GET /sensor] IP Klienta: ");
  Serial.print(server.client().remoteIP().toString()); 
  Serial.print(" | Uptime serwera: ");
  Serial.print(millis());
  Serial.println(" ms");

  float temp = bmp.readTemperature(); 
  int32_t press = bmp.readPressure(); 
  float alt = bmp.readAltitude(); 

  StaticJsonDocument<256> doc; 
  doc["temp"] = temp;
  doc["pressurePa"] = press;
  doc["altitude"] = alt;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);

  requestCount++;
}

void setup() {
  Serial.begin(115200);
  delay(2000); 

  Wire.begin(4, 5); 
  bmp.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Brama Aktywna");

  WiFi.mode(WIFI_AP); 
  WiFi.softAP("Stacja-LAB-NR_INDEXU", "inzynier2026", 1, 1); 

  Serial.println("\n--- BRAMA POWIETRZNA (DELIGHTER) ---");
  Serial.print("Adres IP bramy (AP): ");
  Serial.println(WiFi.softAPIP());

  server.collectHeaders("X-API-KEY");

  server.on("/sensor", handleSensor);
  server.begin();
}

void loop() {
  server.handleClient(); 

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;


    Wire.beginTransmission(0x77);
    if (Wire.endTransmission() == 0) {
      float temp = bmp.readTemperature();
      int32_t press = bmp.readPressure();
      float alt = bmp.readAltitude();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temp, 1);
      lcd.print("C P:");
      lcd.print(press / 100); 
      
      lcd.setCursor(0, 1);
      lcd.print("H:");
      lcd.print(alt, 0);
      lcd.print("m Req:");
      lcd.print(requestCount);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BLAD SENSORA 503");
    }
  }
}