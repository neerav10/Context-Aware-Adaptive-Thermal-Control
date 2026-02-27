#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------- IDs ----------
#define EDGE_ID   0
#define NODE1_ID  1
#define NODE3_ID  3

// ---------- Message Types ----------
#define TYPE_POLL     0
#define TYPE_DATA     1
#define TYPE_COMMAND  2

// ---------- DS18B20 ----------
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ---------- NRF ----------
RF24 radio(9, 8);

const byte ADDR_NODE[] = "NODE1";
const byte ADDR_EDGE[] = "NODE2";

// ---------- Packet ----------
struct Packet {
  uint8_t senderId;
  uint8_t type;
  int value;   // temperature ×10
};

void setup() {
  Serial.begin(9600);
  sensors.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(Packet));
  radio.setAutoAck(false);

  radio.openReadingPipe(1, ADDR_NODE);
  radio.startListening();

  Serial.println("NODE1 READY");
}

void loop() {

  if (radio.available()) {
    Packet pkt;
    radio.read(&pkt, sizeof(pkt));

    if (pkt.type == TYPE_POLL) {

      sensors.requestTemperatures();
      float tempC = sensors.getTempCByIndex(0);

      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Sensor Error");
        return;
      }

      Packet data;
      data.senderId = NODE1_ID;
      data.type = TYPE_DATA;
      data.value = (int)(tempC * 10);

      radio.stopListening();
      radio.openWritingPipe(ADDR_EDGE);
      radio.write(&data, sizeof(data));
      radio.startListening();

      Serial.print("NODE1 SENT TEMP: ");
      Serial.println(tempC);
    }

    if (pkt.type == TYPE_COMMAND) {
      Serial.print("NODE1 COMMAND: ");
      Serial.println(pkt.value);
    }
  }
}
