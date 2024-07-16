#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* firmwareUrl = "https://<seu-usuario>.github.io/<seu-repositorio>/firmware.bin";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Call the function to check and update firmware
  checkForUpdate();
}

void checkForUpdate() {
  WiFiClient client;
  HTTPClient http;

  Serial.println("atualizooooooooooooooooooooooooooooooooo...");
  http.begin(client, firmwareUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Begin OTA update...");
      WiFiClient * stream = http.getStreamPtr();
      size_t written = Update.writeStream(*stream);

      if (written == contentLength) {
        Serial.println("OTA update completed!");
        if (Update.end()) {
          Serial.println("Update successfully applied, restarting...");
          ESP.restart();
        } else {
          Serial.printf("Update failed. Error: %s\n", Update.errorString());
        }
      } else {
        Serial.printf("Written only : %d/%d. Retry?\n", written, contentLength);
      }
    } else {
      Serial.println("Not enough space to begin OTA update");
    }
  } else {
    Serial.printf("HTTP request failed. Error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void loop() {
  // Add any other code you need in the loop
}
