#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define RX_PIN D6   // From Arduino TX
#define TX_PIN D5   // To Arduino RX

SoftwareSerial edgeSerial(RX_PIN, TX_PIN);

const char* ssid = "ND10";
const char* password = "Leomessi10";

const char* host = "172.30.210.210";   // Change to your server
const int port = 5000;

String buffer = "";

void setup() {
  Serial.begin(115200);       // Debug
  edgeSerial.begin(9600);     // From Edge

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());
}

void loop() {

  while (edgeSerial.available()) {
    char c = edgeSerial.read();

    if (c == '\n') {
      buffer.trim();
      processData(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}

void processData(String data) {

  if (!data.startsWith("SEND:"))
    return;

  int commaIndex = data.indexOf(',');
  if (commaIndex == -1)
    return;

  int nodeId = data.substring(5, commaIndex).toInt();
  float temp = data.substring(commaIndex + 1).toFloat();

  String json = "{";
  json += "\"edge\":0,";
  json += "\"node\":";
  json += nodeId;
  json += ",\"temperature\":";
  json += temp;
  json += "}";

  sendToServer(json);
}

void sendToServer(String json) {

  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Server connection failed");
    return;
  }

  Serial.println("Sending JSON:");
  Serial.println(json);

  client.println("POST /data HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.println(json);

  client.stop();
}
