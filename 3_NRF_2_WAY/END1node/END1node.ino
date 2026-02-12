/////////////////////POLLING/////////////////////
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
const byte ADDR_NODE[] = "NODE1";
const byte ADDR_EDGE[] = "NODE2";

// ---------- Packet Structure ----------
struct Packet {
  uint8_t senderId;
  uint8_t type;
  int value;
};

int sensorValue = 100;

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

  Serial.println("NODE1 READY");
}

void loop() {

  if (radio.available()) {
    Packet pkt;
    radio.read(&pkt, sizeof(pkt));

    // EDGE polling
    if (pkt.type == TYPE_POLL) {

      Serial.println("NODE1 POLLED");

      Packet data;
      data.senderId = NODE1_ID;
      data.type = TYPE_DATA;
      data.value = sensorValue++;

      radio.stopListening();
      radio.openWritingPipe(ADDR_EDGE);
      radio.write(&data, sizeof(data));
      radio.startListening();

      Serial.print("NODE1 SENT DATA: ");
      Serial.println(data.value);
    }

    // Command received
    if (pkt.type == TYPE_COMMAND) {

      Serial.print("NODE1 COMMAND RECEIVED: ");
      Serial.println(pkt.value);

      if (pkt.value == 0)
        Serial.println("NODE1 STATUS: NORMAL");
      else if (pkt.value == 1)
        Serial.println("NODE1 STATUS: WARNING");
      else if (pkt.value == 2)
        Serial.println("NODE1 STATUS: CRITICAL");
    }
  }
}



















//////////////////////////------------------TDMA FIXED-----------------------//////////////////////////
//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE1[] = "NODE1";
//const byte ADDR_EDGE[] = "NODE2";
//
//int txValue = 100;
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
//  Serial.println("NODE1 READY");
//}
//
//void loop() {
//  int slotIndex = (millis() / 1000) % 4;
//
//  // SLOT 0: NODE1 → EDGE (send ONCE)
//  if (slotIndex == 0 && lastSlot != 0) {
//    lastSlot = 0;
//
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_EDGE);
//    radio.write(&txValue, sizeof(txValue));
//
//    Serial.print("NODE1 SENT: ");
//    Serial.println(txValue);
//    txValue++;
//  }
//
//  // SLOT 1: NODE1 ← EDGE (receive)
//  if (slotIndex == 1) {
//    radio.openReadingPipe(1, ADDR_NODE1);
//    radio.startListening();
//
//    if (radio.available()) {
//      int rx;
//      radio.read(&rx, sizeof(rx));
//      Serial.print("NODE1 RECEIVED: ");
//      Serial.println(rx);
//    }
//  }
//
//  if (slotIndex != lastSlot) {
//    if (slotIndex != 0) lastSlot = -1;
//  }
//}
















////////////////////////////---------------------FOR TESTING PURPOSE KINDLY DELETE THIS CODE IF NOT REQUIRED-----------------/////////////////////////////

//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE1[] = "NODE1";
//const byte ADDR_NODE2[] = "NODE2";
//
//int txValue = 1;
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
//  Serial.println("NODE1 READY");
//}
//
//void loop() {
//  unsigned long slot = millis() % 4000;
//
//  // SLOT A: NODE1 → NODE2
//  if (slot < 1000) {
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE2);
//
//    radio.write(&txValue, sizeof(txValue));
//    Serial.print("NODE1 SENT: ");
//    Serial.println(txValue);
//
//    txValue++;
//    delay(200);
//  }
//
//  // SLOT B: NODE1 ← NODE2
//  else if (slot < 2000) {
//    radio.openReadingPipe(1, ADDR_NODE1);
//    radio.startListening();
//
//    if (radio.available()) {
//      int rx;
//      radio.read(&rx, sizeof(rx));
//      Serial.print("NODE1 RECEIVED: ");
//      Serial.println(rx);
//    }
//  }
//}
