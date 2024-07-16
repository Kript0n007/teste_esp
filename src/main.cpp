#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>

void checkForUpdate();
void performOTA();

const char* ssid = "KDM - QUINCHO";
const char* password = "antonio2023";
const char* versionUrl = "https://kript0n007.github.io/teste_esp/version.txt";
const char* firmwareUrl = "https://kript0n007.github.io/teste_esp/firmware.bin";
const char* currentVersion = "1.1"; // Versão atual do firmware

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  checkForUpdate();  // Chama a função aqui
}

void checkForUpdate() {
  WiFiClientSecure client;  // Use WiFiClientSecure para HTTPS
  HTTPClient http;

  Serial.println("Checking for firmware version...");
  client.setInsecure();  // Desabilitar verificação de certificado para simplicidade
  http.begin(client, versionUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String newVersion = http.getString();

    newVersion.trim(); // Remove whitespace

    if (newVersion != currentVersion) {
      Serial.printf("New firmware version available: %s\n", newVersion.c_str());
      performOTA();
    } else {
      Serial.println("Firmware is up to date.");
    }
  } else {
    Serial.printf("HTTP request failed. Error: %s\n", http.errorToString(httpCode).c_str());
    Serial.printf("HTTP error code: %d\n", httpCode);
  }
  http.end();
}

void performOTA() {
  WiFiClientSecure client;  // Use WiFiClientSecure para HTTPS
  HTTPClient http;

  Serial.println("Checking for firmware updateeeeeeeeeeeeeee...");
  client.setInsecure();  // Desabilitar verificação de certificado para simplicidade
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // Configura para seguir redirecionamentos
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
    Serial.printf("HTTP error code: %d\n", httpCode);
  }
  http.end();
}

void loop() {
  // Adicione o código principal aqui
}
