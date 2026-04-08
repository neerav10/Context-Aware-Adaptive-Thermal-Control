//---------------APPLICATION------------------


//
//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//// ---------- IDs ----------
//#define EDGE_ID   0
//#define NODE1_ID  1
//#define NODE3_ID  3
//
//// ---------- Message Types ----------
//#define TYPE_POLL     0
//#define TYPE_DATA     1
//#define TYPE_COMMAND  2
//
//RF24 radio(9, 8);
//
//const byte ADDR_EDGE[]  = "NODE2";
//const byte ADDR_NODE1[] = "NODE1";
//const byte ADDR_NODE3[] = "NODE3";
//
//struct Packet {
//  uint8_t senderId;
//  uint8_t type;
//  int value;   // temp ×10
//};
//
//unsigned long lastPoll = 0;
//uint8_t currentNode = NODE1_ID;
//
//void setup() {
//  Serial.begin(9600);   // UART to ESP8266
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(Packet));
//  radio.setAutoAck(false);
//
//  radio.openReadingPipe(1, ADDR_EDGE);
//  radio.startListening();
//
//  Serial.println("EDGE READY");
//}
//
//void pollNode(const byte* addr, uint8_t nodeId) {
//  Packet poll;
//  poll.senderId = EDGE_ID;
//  poll.type = TYPE_POLL;
//  poll.value = 0;
//
//  radio.stopListening();
//  radio.openWritingPipe(addr);
//  radio.write(&poll, sizeof(poll));
//  radio.startListening();
//}
//
//void loop() {
//
//  unsigned long now = millis();
//
//  if (now - lastPoll >= 2000) {
//    lastPoll = now;
//
//    if (currentNode == NODE1_ID) {
//      pollNode(ADDR_NODE1, NODE1_ID);
//      currentNode = NODE3_ID;
//    } else {
//      pollNode(ADDR_NODE3, NODE3_ID);
//      currentNode = NODE1_ID;
//    }
//  }
//
//  if (radio.available()) {
//    Packet pkt;
//    radio.read(&pkt, sizeof(pkt));
//
//    if (pkt.type == TYPE_DATA) {
//
//      float tempC = pkt.value / 10.0;
//
//      // ---- SEND TO ESP ----
//      Serial.print("SEND:");
//      Serial.print(pkt.senderId);
//      Serial.print(",");
//      Serial.println(tempC);
//
//      // ---- Command Logic ----
//      Packet cmd;
//      cmd.senderId = EDGE_ID;
//      cmd.type = TYPE_COMMAND;
//
//      if (tempC < 28.0)
//        cmd.value = 0;
//      else if (tempC < 35.0)
//        cmd.value = 1;
//      else
//        cmd.value = 2;
//
//      radio.stopListening();
//
//      if (pkt.senderId == NODE1_ID)
//        radio.openWritingPipe(ADDR_NODE1);
//      else
//        radio.openWritingPipe(ADDR_NODE3);
//
//      radio.write(&cmd, sizeof(cmd));
//      radio.startListening();
//    }
//  }
//}







//------------ENCRYPTION--------------------------
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <AESLib.h>

// ---------- IDs & Types ----------
#define EDGE_ID      0
#define NODE1_ID     1
#define NODE3_ID     3
#define TYPE_POLL    0
#define TYPE_DATA    1
#define TYPE_COMMAND 2

// ---------- Shared crypto credentials ----------
static uint8_t AES_KEY[16] = {
  0x2B,0x7E,0x15,0x16, 0x28,0xAE,0xD2,0xA6,
  0xAB,0xF7,0x15,0x88, 0x09,0xCF,0x4F,0x3C
};

static const uint8_t AES_IV_MASTER[16] = {
  0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B, 0x0C,0x0D,0x0E,0x0F
};

AESLib aesLib;

// ---------- Packet — 16 bytes ----------
struct Packet {
  uint8_t  senderId;
  uint8_t  type;
  int16_t  value;
  uint8_t  pad[12];
};

RF24 radio(9, 8);
const byte ADDR_EDGE[]  = "NODE2";
const byte ADDR_NODE1[] = "NODE1";
const byte ADDR_NODE3[] = "NODE3";

unsigned long lastPoll = 0;
uint8_t currentNode    = NODE1_ID;

// ---------- Crypto helpers ----------
void encryptPacket(const Packet* src, uint8_t* out) {
  uint8_t iv[16];
  memcpy(iv, AES_IV_MASTER, 16);
  aesLib.encrypt((byte*)src, sizeof(Packet), out, AES_KEY, 128, iv);
}

bool decryptPacket(const uint8_t* in, Packet* dst) {
  uint8_t iv[16];
  memcpy(iv, AES_IV_MASTER, 16);
  uint8_t buf[16];
  memcpy(buf, in, 16);
  aesLib.decrypt(buf, sizeof(Packet), (byte*)dst, AES_KEY, 128, iv);
  return true;
}

// ---------- Poll helper ----------
// Builds a POLL packet, encrypts it, and transmits to the given address.
void pollNode(const byte* addr) {
  Packet poll;
  memset(&poll, 0, sizeof(poll));
  poll.senderId = EDGE_ID;
  poll.type     = TYPE_POLL;
  poll.value    = 0;

  uint8_t enc[16];
  encryptPacket(&poll, enc);

  radio.stopListening();
  radio.openWritingPipe(addr);
  radio.write(enc, 16);
  radio.startListening();
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(16);
  radio.setAutoAck(false);
  radio.openReadingPipe(1, ADDR_EDGE);
  radio.startListening();

  Serial.println("EDGE READY (AESLib CBC)");
}

// ---------- Loop ----------
void loop() {
  unsigned long now = millis();

  // ---- Alternate polling NODE1 and NODE3 every 2 seconds ----
  if (now - lastPoll >= 2000) {
    lastPoll = now;
    if (currentNode == NODE1_ID) {
      pollNode(ADDR_NODE1);
      currentNode = NODE3_ID;
    } else {
      pollNode(ADDR_NODE3);
      currentNode = NODE1_ID;
    }
  }

  // ---- Receive, decrypt, forward to ESP, send command back ----
  if (radio.available()) {
    uint8_t raw[16];
    radio.read(raw, 16);

    Packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    decryptPacket(raw, &pkt);

    if (pkt.type == TYPE_DATA) {
      float tempC = pkt.value / 10.0;

      // Forward plaintext reading to ESP8266 over UART
      Serial.print("SEND:");
      Serial.print(pkt.senderId);
      Serial.print(",");
      Serial.println(tempC);

      // ---- Decide control state from temperature thresholds ----
      Packet cmd;
      memset(&cmd, 0, sizeof(cmd));
      cmd.senderId = EDGE_ID;
      cmd.type     = TYPE_COMMAND;

      if      (tempC < 28.0) cmd.value = 0;   // low gain
      else if (tempC < 35.0) cmd.value = 1;   // high gain
      else                   cmd.value = 2;   // full speed

      uint8_t enc[16];
      encryptPacket(&cmd, enc);

      // Send command back to whichever node sent the data
      radio.stopListening();
      if (pkt.senderId == NODE1_ID)
        radio.openWritingPipe(ADDR_NODE1);
      else
        radio.openWritingPipe(ADDR_NODE3);
      radio.write(enc, 16);
      radio.startListening();
    }
  }
}
