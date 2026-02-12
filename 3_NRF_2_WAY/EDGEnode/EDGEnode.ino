//////////////////////POLLING///////////////////////////////////////
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
const byte ADDR_EDGE[]  = "NODE2";
const byte ADDR_NODE1[] = "NODE1";
const byte ADDR_NODE3[] = "NODE3";

// ---------- Packet Structure ----------
struct Packet {
  uint8_t senderId;
  uint8_t type;
  int value;
};

unsigned long lastPollTime = 0;
uint8_t currentNode = NODE1_ID;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(Packet));
  radio.setAutoAck(false);

  radio.openReadingPipe(1, ADDR_EDGE);
  radio.startListening();

  Serial.println("EDGE READY (Polling Mode)");
}

void pollNode(const byte* address, uint8_t nodeId) {
  Packet poll;
  poll.senderId = EDGE_ID;
  poll.type = TYPE_POLL;
  poll.value = 0;

  radio.stopListening();
  radio.openWritingPipe(address);
  radio.write(&poll, sizeof(poll));
  radio.startListening();

  Serial.print("EDGE POLLED NODE ");
  Serial.println(nodeId);
}

void loop() {

  unsigned long now = millis();

  // Poll every 1000 ms
  if (now - lastPollTime >= 1000) {
    lastPollTime = now;

    if (currentNode == NODE1_ID) {
      pollNode(ADDR_NODE1, NODE1_ID);
      currentNode = NODE3_ID;
    } else {
      pollNode(ADDR_NODE3, NODE3_ID);
      currentNode = NODE1_ID;
    }
  }

  // Receive DATA
  if (radio.available()) {
    Packet pkt;
    radio.read(&pkt, sizeof(pkt));

    if (pkt.type == TYPE_DATA) {

      Serial.print("EDGE RX DATA from NODE ");
      Serial.print(pkt.senderId);
      Serial.print(" = ");
      Serial.println(pkt.value);

      // -------- Decision Logic (0 / 1 / 2) --------
      Packet cmd;
      cmd.senderId = EDGE_ID;
      cmd.type = TYPE_COMMAND;

      if (pkt.value < 120)
        cmd.value = 0;     // NORMAL
      else if (pkt.value < 180)
        cmd.value = 1;     // WARNING
      else
        cmd.value = 2;     // CRITICAL

      // Send command back to sender
      radio.stopListening();

      if (pkt.senderId == NODE1_ID)
        radio.openWritingPipe(ADDR_NODE1);
      else if (pkt.senderId == NODE3_ID)
        radio.openWritingPipe(ADDR_NODE3);

      radio.write(&cmd, sizeof(cmd));
      radio.startListening();

      Serial.print("EDGE SENT COMMAND: ");
      Serial.println(cmd.value);
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
//const byte ADDR_NODE3[] = "NODE3";
//
//int lastFromNode1 = 0;
//int lastFromNode3 = 0;
//int lastSlot = -1;
//int prevSlot = -1;
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
//  Serial.println("EDGE NODE (UNO) READY");
//}
//
//void loop() {
//  int slotIndex = (millis() / 1000) % 4;
//
//  // ---- SLOT HEADER ----
//  if (slotIndex != prevSlot) {
//    Serial.println();
//    Serial.print("=== SLOT ");
//    Serial.print(slotIndex);
//    Serial.print(" START @ ");
//    Serial.print(millis());
//    Serial.println(" ms ===");
//    prevSlot = slotIndex;
//  }
//
//  // SLOT 0: RX from NODE1
//  if (slotIndex == 0) {
//    radio.openReadingPipe(1, ADDR_EDGE);
//    radio.startListening();
//
//    if (radio.available()) {
//      radio.read(&lastFromNode1, sizeof(lastFromNode1));
//      Serial.print(millis());
//      Serial.print(" ms | EDGE RX NODE1: ");
//      Serial.println(lastFromNode1);
//    }
//  }
//
//  // SLOT 1: TX to NODE1
//  if (slotIndex == 1 && lastSlot != 1) {
//    lastSlot = 1;
//
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE1);
//    radio.write(&lastFromNode1, sizeof(lastFromNode1));
//
//    Serial.print(millis());
//    Serial.print(" ms | EDGE TX NODE1: ");
//    Serial.println(lastFromNode1);
//  }
//
//  // SLOT 2: RX from NODE3
//  if (slotIndex == 2) {
//    radio.openReadingPipe(1, ADDR_EDGE);
//    radio.startListening();
//
//    if (radio.available()) {
//      radio.read(&lastFromNode3, sizeof(lastFromNode3));
//      Serial.print(millis());
//      Serial.print(" ms | EDGE RX NODE3: ");
//      Serial.println(lastFromNode3);
//    }
//  }
//
//  // SLOT 3: TX to NODE3
//  if (slotIndex == 3 && lastSlot != 3) {
//    lastSlot = 3;
//
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE3);
//    radio.write(&lastFromNode3, sizeof(lastFromNode3));
//
//    Serial.print(millis());
//    Serial.print(" ms | EDGE TX NODE3: ");
//    Serial.println(lastFromNode3);
//  }
//
//  if (slotIndex != lastSlot && slotIndex != 1 && slotIndex != 3) {
//    lastSlot = -1;
//  }
//}
























////////////////////////////------------------------------------------------------/////////////////////////////

//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE1[] = "NODE1";
//const byte ADDR_NODE2[] = "NODE2";
//const byte ADDR_NODE3[] = "NODE3";
//
//int lastFromNode1 = 0;
//int lastFromNode3 = 0;
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
//  Serial.println("EDGE NODE (UNO) READY");
//}
//
//void loop() {
//  unsigned long slot = millis() % 4000;
//
//  // SLOT A: RX from NODE1
//  if (slot < 1000) {
//    radio.openReadingPipe(1, ADDR_NODE2);
//    radio.startListening();
//
//    if (radio.available()) {
//      radio.read(&lastFromNode1, sizeof(lastFromNode1));
//      Serial.print("EDGE RX NODE1: ");
//      Serial.println(lastFromNode1);
//    }
//  }
//
//  // SLOT B: TX to NODE1
//  else if (slot < 2000) {
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE1);
//
//    radio.write(&lastFromNode1, sizeof(lastFromNode1));
//    Serial.print("EDGE TX NODE1: ");
//    Serial.println(lastFromNode1);
//
//    delay(200);
//  }
//
//  // SLOT C: RX from NODE3
//  else if (slot < 3000) {
//    radio.openReadingPipe(1, ADDR_NODE2);
//    radio.startListening();
//
//    if (radio.available()) {
//      radio.read(&lastFromNode3, sizeof(lastFromNode3));
//      Serial.print("EDGE RX NODE3: ");
//      Serial.println(lastFromNode3);
//    }
//  }
//
//  // SLOT D: TX to NODE3
//  else {
//    radio.stopListening();
//    radio.openWritingPipe(ADDR_NODE3);
//
//    radio.write(&lastFromNode3, sizeof(lastFromNode3));
//    Serial.print("EDGE TX NODE3: ");
//    Serial.println(lastFromNode3);
//
//    delay(200);
//  }
//}
