////////////////////////////////POLLING///////////////////////
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// ---------- IDs ----------
#define EDGE_ID   0
#define NODE1_ID  1
#define NODE3_ID  3

// ---------- Message Types ----------
#define TYPE_POLL     0
#define TYPE_DATA     1
#define TYPE_COMMAND  2

RF24 radio(9, 8);

// ---------- Addresses ----------
const byte ADDR_NODE[] = "NODE3";
const byte ADDR_EDGE[] = "NODE2";

// ---------- Packet Structure ----------
struct Packet {
  uint8_t senderId;
  uint8_t type;
  int value;
};

int sensorValue = 170;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(Packet));
  radio.setAutoAck(false);

  radio.openReadingPipe(1, ADDR_NODE);
  radio.startListening();

  Serial.println("NODE3 READY");
}

void loop() {

  if (radio.available()) {
    Packet pkt;
    radio.read(&pkt, sizeof(pkt));

    if (pkt.type == TYPE_POLL) {

      Serial.println("NODE3 POLLED");

      Packet data;
      data.senderId = NODE3_ID;
      data.type = TYPE_DATA;
      data.value = sensorValue++;

      radio.stopListening();
      radio.openWritingPipe(ADDR_EDGE);
      radio.write(&data, sizeof(data));
      radio.startListening();

      Serial.print("NODE3 SENT DATA: ");
      Serial.println(data.value);
    }

    if (pkt.type == TYPE_COMMAND) {

      Serial.print("NODE3 COMMAND RECEIVED: ");
      Serial.println(pkt.value);

      if (pkt.value == 0)
        Serial.println("NODE3 STATUS: NORMAL");
      else if (pkt.value == 1)
        Serial.println("NODE3 STATUS: WARNING");
      else if (pkt.value == 2)
        Serial.println("NODE3 STATUS: CRITICAL");
    }
  }
}




////////////////////////////------------------TDMA FIXED-----------------------//////////////////////////
//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE3[] = "NODE3";
//const byte ADDR_EDGE[] = "NODE2";
//
//int txValue = 170;
//int lastSlot = -1;
//
//void setup() {
//  Serial.begin(9600);
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(int));
//  radio.setAutoAck(false);
//
//  Serial.println("NODE3 READY");
//}
//
//void loop() {
//  int slotIndex = (millis() / 1000) % 4;
//
//  // SLOT 2: NODE3 → EDGE (send ONCE)
//  if (slotIndex == 2 && lastSlot != 2) {
//    lastSlot = 2;
//
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_EDGE);
//    radio.write(&txValue, sizeof(txValue));
//
//    Serial.print("NODE3 SENT: ");
//    Serial.println(txValue);
//    txValue++;
//  }
//
//  // SLOT 3: NODE3 ← EDGE (receive)
//  if (slotIndex == 3) {
//    radio.openReadingPipe(1, ADDR_NODE3);
//    radio.startListening();
//
//    if (radio.available()) {
//      int rx;
//      radio.read(&rx, sizeof(rx));
//      Serial.print("NODE3 RECEIVED: ");
//      Serial.println(rx);
//    }
//  }
//
//  if (slotIndex != lastSlot) {
//    if (slotIndex != 2) lastSlot = -1;
//  }
//}
//






































////////////////////////////------------------------------------------------------/////////////////////////////





































//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE3[] = "NODE3";
//const byte ADDR_NODE2[] = "NODE2";
//
//int txValue = 100;
//
//void setup() {
//  Serial.begin(9600);
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(int));
//  radio.setAutoAck(false);
//
//  Serial.println("NODE3 READY");
//}
//
//void loop() {
//  unsigned long slot = millis() % 4000;
//
//  // SLOT C: NODE3 → NODE2
//  if (slot >= 2000 && slot < 3000) {
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE2);
//
//    radio.write(&txValue, sizeof(txValue));
//    Serial.print("NODE3 SENT: ");
//    Serial.println(txValue);
//
//    txValue++;
//    delay(200);
//  }
//
//  // SLOT D: NODE3 ← NODE2
//  else if (slot >= 3000) {
//    radio.openReadingPipe(1, ADDR_NODE3);
//    radio.startListening();
//
//    if (radio.available()) {
//      int rx;
//      radio.read(&rx, sizeof(rx));
//      Serial.print("NODE3 RECEIVED: ");
//      Serial.println(rx);
//    }
//  }
//}
