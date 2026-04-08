//--------------------TESTING-----------------//
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
//#define ONE_WIRE_BUS 2
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//
//RF24 radio(9, 8);
//
//const byte ADDR_NODE[] = "NODE3";
//const byte ADDR_EDGE[] = "NODE2";
//
//struct Packet {
//  uint8_t senderId;
//  uint8_t type;
//  int value;
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
//  Serial.println("NODE3 READY");
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
//      data.senderId = NODE3_ID;
//      data.type = TYPE_DATA;
//      data.value = (int)(tempC * 10);
//
//      radio.stopListening();
//      radio.openWritingPipe(ADDR_EDGE);
//      radio.write(&data, sizeof(data));
//      radio.startListening();
//
//      Serial.print("NODE3 SENT TEMP: ");
//      Serial.println(tempC);
//    }
//
//    if (pkt.type == TYPE_COMMAND) {
//      Serial.print("NODE3 COMMAND: ");
//      Serial.println(pkt.value);
//    }
//  }
//}


//-------------------------APPLICATION--------------------------------------//
//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//
//#define EDGE_ID 0
//#define NODE3_ID 3
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
//const byte ADDR_NODE[] = "NODE3";
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
//  Serial.println("NODE3 READY");
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
//      data.senderId = NODE3_ID;
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




//--------------------------ENCRYPTION------------------------------------
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AESLib.h>

// ---------- IDs & Types ----------
#define EDGE_ID      0
#define NODE3_ID     3
#define TYPE_POLL    0
#define TYPE_DATA    1
#define TYPE_COMMAND 2
#define MOTOR_PIN    5

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
const byte ADDR_NODE[] = "NODE3";
const byte ADDR_EDGE[] = "NODE2";

float currentTemp = 0;

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
  radio.setPayloadSize(16);
  radio.setAutoAck(false);
  radio.openReadingPipe(1, ADDR_NODE);
  radio.startListening();

  Serial.println("NODE3 READY (AESLib CBC)");
}

// ---------- Loop ----------
void loop() {
  if (radio.available()) {
    uint8_t raw[16];
    radio.read(raw, 16);

    Packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    decryptPacket(raw, &pkt);

    // ---- Poll: read sensor and reply ----
    if (pkt.type == TYPE_POLL) {
      sensors.requestTemperatures();
      currentTemp = sensors.getTempCByIndex(0);

      Packet data;
      memset(&data, 0, sizeof(data));
      data.senderId = NODE3_ID;
      data.type     = TYPE_DATA;
      data.value    = (int16_t)(currentTemp * 10);

      uint8_t enc[16];
      encryptPacket(&data, enc);

      radio.stopListening();
      radio.openWritingPipe(ADDR_EDGE);
      radio.write(enc, 16);
      radio.startListening();

      Serial.print("TEMP SENT: ");
      Serial.println(currentTemp);
    }

    // ---- Command: drive motor ----
    if (pkt.type == TYPE_COMMAND) {
      int state = (int)pkt.value;
      float TE_ERR  = currentTemp - TE_BASE;
      int targetPWM = 0;

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
