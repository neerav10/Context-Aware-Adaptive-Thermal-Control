////
//
//#include <ESP8266WiFi.h>
//#include <SoftwareSerial.h>
//
//#define RX_PIN D6   // Arduino TX → ESP RX
//#define TX_PIN D5   // ESP TX → Arduino RX
//
//SoftwareSerial edgeSerial(RX_PIN, TX_PIN);
//
//const char* ssid = "ND10";
//const char* password = "Leomessi10";
//
//const char* host = "10.176.59.210";   // Your PC running Flask
//const int port = 5000;
//
//String buffer = "";
//
//void setup()
//{
//  Serial.begin(115200);
//  edgeSerial.begin(9600);
//
//  WiFi.begin(ssid,password);
//
//  Serial.print("Connecting");
//
//  while(WiFi.status()!=WL_CONNECTED)
//  {
//    delay(500);
//    Serial.print(".");
//  }
//
//  Serial.println("\nWiFi Connected");
//  Serial.println(WiFi.localIP());
//}
//
//void loop()
//{
//  while(edgeSerial.available())
//  {
//    char c = edgeSerial.read();
//
//    if(c == '\n')
//    {
//      buffer.trim();
//      processData(buffer);
//      buffer = "";
//    }
//    else
//    {
//      buffer += c;
//    }
//  }
//}
//
//void processData(String data)
//{
//  Serial.println("UART RX: " + data);
//
//  if(!data.startsWith("EDGE"))
//      return;
//
//  int p1 = data.indexOf(',');
//  int p2 = data.indexOf(',',p1+1);
//  int p3 = data.indexOf(',',p2+1);
//
//  if(p1==-1 || p2==-1 || p3==-1)
//      return;
//
//  int nodeId = data.substring(p1+1,p2).toInt();
//  float temp = data.substring(p2+1,p3).toFloat();
//  int state = data.substring(p3+1).toInt();
//
//  String json = "{";
//  json += "\"edge\":0,";
//  json += "\"node\":";
//  json += nodeId;
//  json += ",\"temperature\":";
//  json += temp;
//  json += ",\"state\":";
//  json += state;
//  json += "}";
//
//  sendToServer(json);
//}
//
//void sendToServer(String json)
//{
//  WiFiClient client;
//
//  if(!client.connect(host,port))
//  {
//    Serial.println("Server connection failed");
//    return;
//  }
//
//  Serial.println("Sending JSON:");
//  Serial.println(json);
//
//  client.println("POST /data HTTP/1.1");
//  client.print("Host: ");
//  client.println(host);
//  client.println("Content-Type: application/json");
//  client.print("Content-Length: ");
//  client.println(json.length());
//  client.println();
//  client.println(json);
//
//  client.stop();
//}






#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// 🔌 SoftwareSerial pins
#define RX_PIN D6   // Arduino TX → ESP RX (GPIO12)
#define TX_PIN D5   // ESP TX → Arduino RX (GPIO14)

SoftwareSerial edgeSerial(RX_PIN, TX_PIN);

// 🌐 WiFi
const char* ssid = "ND10";
const char* password = "Leomessi10";

// 🌍 Flask server
const char* host = "10.181.185.210";
const int port = 5000;

String buffer = "";

void setup()
{
  Serial.begin(115200);      // Debug
  edgeSerial.begin(9600);    // MUST stay LOW for stability

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());
}

void loop()
{
  while (edgeSerial.available())
  {
    char c = edgeSerial.read();
    delay(2);   // 🔥 IMPORTANT for SoftwareSerial stability

    if (c == '\n')
    {
      buffer.trim();
      processData(buffer);
      buffer = "";
    }
    else
    {
      buffer += c;
    }
  }
}

// 🔍 Parse incoming data
void processData(String data)
{
  Serial.println("UART RX: " + data);

  // Expected: SEND:1,27.3
  if (!data.startsWith("SEND:"))
      return;

  int p1 = data.indexOf(':');
  int p2 = data.indexOf(',', p1 + 1);

  if (p1 == -1 || p2 == -1)
      return;

  int nodeId = data.substring(p1 + 1, p2).toInt();
  float temp = data.substring(p2 + 1).toFloat();

  // Same logic as Arduino
  int state;
  if      (temp < 28.0) state = 0;
  else if (temp < 35.0) state = 1;
  else                  state = 2;

  // Create JSON
  String json = "{";
  json += "\"edge\":0,";
  json += "\"node\":";
  json += nodeId;
  json += ",\"temperature\":";
  json += temp;
  json += ",\"state\":";
  json += state;
  json += "}";

  sendToServer(json);
}

// 🌐 Send to Flask server
void sendToServer(String json)
{
  WiFiClient client;

  if (!client.connect(host, port))
  {
    Serial.println("❌ Server connection failed");
    return;
  }

  Serial.println("✅ Sending JSON:");
  Serial.println(json);

  client.println("POST /data HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println("Connection: close");
  client.println();
  client.println(json);

  delay(10);

  // Debug response
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  client.stop();
}
