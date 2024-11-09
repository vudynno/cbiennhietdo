#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <ArduinoJson.h> 
#include <WiFiClientSecure.h>

const char* ssid = "NTN";
const char* password = "01490149";

const char* mqtt_server = "47.129.242.217"; 

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(D1); 
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("dang ket noi den WiFi...");
  }
  Serial.println("Ket noi WiFi thanh cong");

  
  client.setServer(mqtt_server, 1883);
  
  if (!rtc.begin()) {
    Serial.println("Khong the tim thay RTC");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  DateTime now = rtc.now();
  String timeString = String(now.year(), DEC) + "-" + String(now.month(), DEC) + "-" + String(now.day(), DEC) + " " + 
                      String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC);

  StaticJsonDocument<100> tempJsonDoc;
  tempJsonDoc["temperature"] = temperature;
  String tempJsonString;
  serializeJson(tempJsonDoc, tempJsonString);

  StaticJsonDocument<100> timeJsonDoc;
  timeJsonDoc["time"] = timeString;
  String timeJsonString;
  serializeJson(timeJsonDoc, timeJsonString);

  client.publish("sensor/temperature", tempJsonString.c_str());
  client.publish("sensor/time", timeJsonString.c_str());

  delay(2000); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("dang co gang ket noi den MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("da ket noi");
    } else {
      Serial.print("Loi, ma loi = ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
