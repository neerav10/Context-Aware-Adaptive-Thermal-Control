//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
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
//// ---------- DS18B20 ----------
//#define ONE_WIRE_BUS 2
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//
//// ---------- NRF ----------
//RF24 radio(9, 8);
//
//const byte ADDR_NODE[] = "NODE1";
//const byte ADDR_EDGE[] = "NODE2";
//
//// ---------- Packet ----------
//struct Packet {
//  uint8_t senderId;
//  uint8_t type;
//  int value;   // temperature ×10
//};
//
//void setup() {
//  Serial.begin(9600);
//  sensors.begin();
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(Packet));
//  radio.setAutoAck(false);
//
//  radio.openReadingPipe(1, ADDR_NODE);
//  radio.startListening();
//
//  Serial.println("NODE1 READY");
//}
//
//void loop() {
//
//  if (radio.available()) {
//    Packet pkt;
//    radio.read(&pkt, sizeof(pkt));
//
//    if (pkt.type == TYPE_POLL) {
//
//      sensors.requestTemperatures();
//      float tempC = sensors.getTempCByIndex(0);
//
//      if (tempC == DEVICE_DISCONNECTED_C) {
//        Serial.println("Sensor Error");
//        return;
//      }
//
//      Packet data;
//      data.senderId = NODE1_ID;
//      data.type = TYPE_DATA;
//      data.value = (int)(tempC * 10);
//
//      radio.stopListening();
//      radio.openWritingPipe(ADDR_EDGE);
//      radio.write(&data, sizeof(data));
//      radio.startListening();
//
//      Serial.print("NODE1 SENT TEMP: ");
//      Serial.println(tempC);
//    }
//
//    if (pkt.type == TYPE_COMMAND) {
//      Serial.print("NODE1 COMMAND: ");
//      Serial.println(pkt.value);
//    }
//  }
//}






//-----------------------------APPLICATION--------------------------------------//

//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//
//#define EDGE_ID 0
//#define NODE1_ID 1
//
//#define TYPE_POLL 0
//#define TYPE_DATA 1
//#define TYPE_COMMAND 2
//
//#define MOTOR_PIN 5
//
//float TE_BASE = 30.0;
//float CT_K = 8;
//float CT_K2 = 12;
//
//int CT_PWM_MAX = 255;
//int currentPWM = 0;
//
//#define ONE_WIRE_BUS 2
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//
//RF24 radio(9,8);
//
//const byte ADDR_NODE[] = "NODE1";
//const byte ADDR_EDGE[] = "NODE2";
//
//struct Packet{
//  uint8_t senderId;
//  uint8_t type;
//  int value;
//};
//
//float currentTemp = 0;
//
//void smoothMotor(int targetPWM)
//{
//  if(targetPWM > currentPWM) currentPWM += 5;
//  if(targetPWM < currentPWM) currentPWM -= 5;
//
//  analogWrite(MOTOR_PIN,currentPWM);
//}
//
//void setup()
//{
//  Serial.begin(9600);
//
//  sensors.begin();
//
//  pinMode(MOTOR_PIN,OUTPUT);
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(Packet));
//  radio.setAutoAck(false);
//
//  radio.openReadingPipe(1,ADDR_NODE);
//  radio.startListening();
//
//  Serial.println("NODE1 READY");
//}
//
//void loop()
//{
//  if(radio.available())
//  {
//    Packet pkt;
//    radio.read(&pkt,sizeof(pkt));
//
//    if(pkt.type == TYPE_POLL)
//    {
//      sensors.requestTemperatures();
//      currentTemp = sensors.getTempCByIndex(0);
//
//      Packet data;
//
//      data.senderId = NODE1_ID;
//      data.type = TYPE_DATA;
//      data.value = (int)(currentTemp * 10);
//
//      radio.stopListening();
//      radio.openWritingPipe(ADDR_EDGE);
//      radio.write(&data,sizeof(data));
//      radio.startListening();
//
//      Serial.print("TEMP SENT: ");
//      Serial.println(currentTemp);
//    }
//
//    if(pkt.type == TYPE_COMMAND)
//    {
//      int state = pkt.value;
//
//      float TE_ERR = currentTemp - TE_BASE;
//
//      int targetPWM = 0;
//
//      if(state == 0)
//          targetPWM = CT_K * TE_ERR;
//
//      else if(state == 1)
//          targetPWM = CT_K2 * TE_ERR;
//
//      else if(state == 2)
//          targetPWM = CT_PWM_MAX;
//
//      if(targetPWM < 0) targetPWM = 0;
//      if(targetPWM > 255) targetPWM = 255;
//
//      smoothMotor(targetPWM);
//
//      Serial.print("STATE: ");
//      Serial.print(state);
//      Serial.print(" PWM: ");
//      Serial.println(currentPWM);
//    }
//  }
//}





//-------------------------------------ENCRYPTION---------------------------------------

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AESLib.h>               // Matej Sychra — install "AESLib" in Library Manager

// ---------- IDs & Types ----------
#define EDGE_ID      0
#define NODE1_ID     1
#define TYPE_POLL    0
#define TYPE_DATA    1
#define TYPE_COMMAND 2
#define MOTOR_PIN    5

// ---------- Shared crypto credentials ----------
// Must be identical on all three nodes.
// AES_KEY is declared as non-const because AESLib's API takes a non-const pointer.
static uint8_t AES_KEY[16] = {
  0x2B,0x7E,0x15,0x16, 0x28,0xAE,0xD2,0xA6,
  0xAB,0xF7,0x15,0x88, 0x09,0xCF,0x4F,0x3C
};

// Master IV — NEVER passed directly to AESLib.
// AESLib mutates the IV buffer in place during every call,
// so we copy from this master before every encrypt/decrypt.
static const uint8_t AES_IV_MASTER[16] = {
  0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B, 0x0C,0x0D,0x0E,0x0F
};

AESLib aesLib;                    // single global instance, reused for every call

// ---------- Packet — exactly 16 bytes (one AES-CBC block) ----------
struct Packet {
  uint8_t  senderId;   // 1 byte
  uint8_t  type;       // 1 byte
  int16_t  value;      // 2 bytes  (int16_t guarantees size on all compilers)
  uint8_t  pad[12];    // 12 bytes padding → total 16 bytes
};

// ---------- Temperature control ----------
float TE_BASE    = 30.0;
float CT_K       = 8;
float CT_K2      = 12;
int   CT_PWM_MAX = 255;
int   currentPWM = 0;

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

RF24 radio(9, 8);
const byte ADDR_NODE[] = "NODE1";
const byte ADDR_EDGE[] = "NODE2";

float currentTemp = 0;

// ---------- Crypto helpers ----------

// Encrypts one Packet into 16 raw bytes using AES-128-CBC.
// A fresh IV copy is made before every call because AESLib
// overwrites the IV buffer during encryption.
void encryptPacket(const Packet* src, uint8_t* out) {
  uint8_t iv[16];
  memcpy(iv, AES_IV_MASTER, 16);            // restore IV from master
  aesLib.encrypt(
    (byte*)src,                             // plaintext input
    sizeof(Packet),                         // must be exactly 16 bytes
    out,                                    // ciphertext output
    AES_KEY,                                // 16-byte key
    128,                                    // key size in bits
    iv                                      // IV (will be mutated — that's why it's a copy)
  );
}

// Decrypts 16 raw bytes back into a Packet using AES-128-CBC.
// Same IV-copy discipline applies on the receiver side.
bool decryptPacket(const uint8_t* in, Packet* dst) {
  uint8_t iv[16];
  memcpy(iv, AES_IV_MASTER, 16);            // restore IV from master

  uint8_t buf[16];
  memcpy(buf, in, 16);                      // work on a local copy of ciphertext

  aesLib.decrypt(
    buf,                                    // ciphertext input
    sizeof(Packet),                         // 16 bytes
    (byte*)dst,                             // plaintext output (directly into Packet)
    AES_KEY,
    128,
    iv
  );
  return true;
}

// ---------- Motor ----------
void smoothMotor(int targetPWM) {
  if (targetPWM > currentPWM) currentPWM += 5;
  if (targetPWM < currentPWM) currentPWM -= 5;
  analogWrite(MOTOR_PIN, currentPWM);
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(MOTOR_PIN, OUTPUT);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(16);                 // one AES block exactly
  radio.setAutoAck(false);
  radio.openReadingPipe(1, ADDR_NODE);
  radio.startListening();

  Serial.println("NODE1 READY (AESLib CBC)");
}

// ---------- Loop ----------
void loop() {
  if (radio.available()) {
    uint8_t raw[16];
    radio.read(raw, 16);

    Packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    decryptPacket(raw, &pkt);               // decrypt raw bytes → Packet struct

    // ---- Poll: read sensor and reply with temperature ----
    if (pkt.type == TYPE_POLL) {
      sensors.requestTemperatures();
      currentTemp = sensors.getTempCByIndex(0);

      Packet data;
      memset(&data, 0, sizeof(data));       // zero pad bytes before filling
      data.senderId = NODE1_ID;
      data.type     = TYPE_DATA;
      data.value    = (int16_t)(currentTemp * 10);  // e.g. 27.3°C → 273

      uint8_t enc[16];
      encryptPacket(&data, enc);            // encrypt Packet → 16 raw bytes

      radio.stopListening();
      radio.openWritingPipe(ADDR_EDGE);
      radio.write(enc, 16);
      radio.startListening();

      Serial.print("TEMP SENT: ");
      Serial.println(currentTemp);
    }

    // ---- Command: set motor speed based on state from EDGE ----
    if (pkt.type == TYPE_COMMAND) {
      int state = (int)pkt.value;
      float TE_ERR   = currentTemp - TE_BASE;
      int targetPWM  = 0;

      if      (state == 0) targetPWM = (int)(CT_K  * TE_ERR);
      else if (state == 1) targetPWM = (int)(CT_K2 * TE_ERR);
      else if (state == 2) targetPWM = CT_PWM_MAX;

      targetPWM = constrain(targetPWM, 0, 255);
      smoothMotor(targetPWM);

      Serial.print("STATE: "); Serial.print(state);
      Serial.print("  PWM: "); Serial.println(currentPWM);
    }
  }
}
