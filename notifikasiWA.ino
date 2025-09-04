#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <UrlEncode.h>

const char* ssid = "Vhitry";
const char* password = "fitri123";
String phoneNumber = "6281312330594";
String apiKey = "6132890";

SoftwareSerial mySerial(D7, D8); // RX, TX
WiFiClientSecure wifiClient;

void sendMessage(String message) {
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber +
               "&text=" + urlEncode(message) +
               "&apikey=" + apiKey;

  wifiClient.setInsecure();
  HTTPClient http;
  http.begin(wifiClient, url);
  int httpResponseCode = http.GET();
  http.end();

  if (httpResponseCode == 200) {
    Serial.println("✅ WhatsApp message sent.");
  } else {
    Serial.println("❌ Gagal kirim pesan. Kode HTTP:");
    Serial.println(httpResponseCode);
  }
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

void loop() {
  if (mySerial.available()) {
    String jsonData = mySerial.readStringUntil('\n');
    Serial.println("Diterima dari Mega:");
    Serial.println(jsonData);

    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, jsonData);
    if (err) {
      Serial.print("Parsing gagal: ");
      Serial.println(err.f_str());
      return;
    }

    String org = doc["volume_organik"];
    String unorg = doc["volume_unorganik"];

    if (org == "Sampah Organik Penuh") {
      sendMessage("⚠️ Sampah Organik PENUH. Segera kosongkan.");
    }

    if (unorg == "Sampah Unorganik Penuh") {
      sendMessage("⚠️ Sampah Unorganik PENUH. Segera kosongkan.");
    }
  }
}
