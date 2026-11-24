#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>

// D5(GPIO14) = RX, D6(GPIO12) = TX
SoftwareSerial unoSerial(D5, D6);

const char* WIFI_SSID = "iPhone13";
const char* WIFI_PASSWORD = "pms020208";
const char* SERVER_URL = "https://busgil.xyz/";

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Wi-Fi 연결 중");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("연결 성공! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(WiFi.status());
    Serial.println("Wi-Fi 연결 실패 (다음 주기에 재시도)");
  }
}

void sendSensorData(int sensorValue) {
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(WiFi.status());
    Serial.println("Wi-Fi 미연결로 전송 건너뜀");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(10000);
  HTTPClient http;
  http.setTimeout(10000);

  http.setTimeout(5000);
  if (!http.begin(client, SERVER_URL)) {
    Serial.println("HTTP 초기화 실패");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  String jsonBody = "{\"device\":\"uno_nodemcu_01\",\"sensor_value\":" + String(sensorValue) + "}";

  int httpCode = http.POST(jsonBody);
  Serial.print("HTTP 응답 코드: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("서버 응답: " + response);
  } else {
    Serial.print("전송 오류: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
  Serial.println("NodeMCU V3 와이파이 게이트웨이 시작");
  connectWiFi();
}

void loop() {
  connectWiFi();

  if (unoSerial.available()) {
    String line = unoSerial.readStringUntil('\n');
    line.trim();

    if (line.startsWith("SENSOR:")) {
      String numStr = line.substring(7);
      numStr.trim();
      int sensorValue = numStr.toInt();

      Serial.println("수신된 센서 값: " + String(sensorValue));
      sendSensorData(sensorValue);
    }
  }
}