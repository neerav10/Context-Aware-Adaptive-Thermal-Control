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
//  Serial.begin(9600);
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
//
//  Serial.print("EDGE POLLED NODE ");
//  Serial.println(nodeId);
//}
//
//void loop() {
//
//  unsigned long now = millis();
//
//  // Poll alternately every 2 seconds
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
//      Serial.print("EDGE RX from NODE ");
//      Serial.print(pkt.senderId);
//      Serial.print(": ");
//      Serial.print(tempC);
//      Serial.println(" C");
//
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
//
//      Serial.print("EDGE SENT COMMAND: ");
//      Serial.println(cmd.value);
//    }
//  }
//}





//---------------------------------------------------------------------//
//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//#define EDGE_ID 0
//#define NODE1_ID 1
//#define NODE3_ID 3
//
//#define TYPE_POLL 0
//#define TYPE_DATA 1
//#define TYPE_COMMAND 2
//
//RF24 radio(9,8);
//
//const byte ADDR_EDGE[] = "NODE2";
//const byte ADDR_NODE1[] = "NODE1";
//const byte ADDR_NODE3[] = "NODE3";
//
//struct Packet{
//  uint8_t senderId;
//  uint8_t type;
//  int value;
//};
//
//float TE_LIM_HIGH = 40.0;
//float TE_LIM_LOW = 38.0;
//
//float TE_FIL = 0;
//
//unsigned long TI_CROSS = 0;
//unsigned long TI_TH = 5000;
//
//unsigned long lastPoll = 0;
//int currentNode = NODE1_ID;
//
//void setup()
//{
//  Serial.begin(9600);
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_LOW);
//  radio.setDataRate(RF24_250KBPS);
//  radio.setChannel(76);
//  radio.disableDynamicPayloads();
//  radio.setPayloadSize(sizeof(Packet));
//  radio.setAutoAck(false);
//
//  radio.openReadingPipe(1,ADDR_EDGE);
//  radio.startListening();
//
//  Serial.println("EDGE READY");
//}
//
//void pollNode(const byte* addr)
//{
//  Packet poll;
//
//  poll.senderId = EDGE_ID;
//  poll.type = TYPE_POLL;
//  poll.value = 0;
//
//  radio.stopListening();
//  radio.openWritingPipe(addr);
//  radio.write(&poll,sizeof(poll));
//  radio.startListening();
//}
//
//void sendCommand(uint8_t node,int state)
//{
//  Packet cmd;
//
//  cmd.senderId = EDGE_ID;
//  cmd.type = TYPE_COMMAND;
//  cmd.value = state;
//
//  radio.stopListening();
//
//  if(node == NODE1_ID)
//      radio.openWritingPipe(ADDR_NODE1);
//  else
//      radio.openWritingPipe(ADDR_NODE3);
//
//  radio.write(&cmd,sizeof(cmd));
//  radio.startListening();
//}
//
//void loop()
//{
//  unsigned long now = millis();
//
//  if(now - lastPoll > 2000)
//  {
//    lastPoll = now;
//
//    if(currentNode == NODE1_ID)
//    {
//      pollNode(ADDR_NODE1);
//      currentNode = NODE3_ID;
//    }
//    else
//    {
//      pollNode(ADDR_NODE3);
//      currentNode = NODE1_ID;
//    }
//  }
//
//  if(radio.available())
//  {
//    Packet pkt;
//    radio.read(&pkt,sizeof(pkt));
//
//    if(pkt.type == TYPE_DATA)
//    {
//      float temp = pkt.value / 10.0;
//
//      Serial.print("TEMP RX: ");
//      Serial.println(temp);
//
//      TE_FIL = 0.7 * TE_FIL + 0.3 * temp;
//
//      int state = 0;
//
//      if(TE_FIL > TE_LIM_HIGH)
//      {
//        if(TI_CROSS == 0)
//            TI_CROSS = now;
//
//        unsigned long dur = now - TI_CROSS;
//
//        if(dur < TI_TH)
//            state = 1;
//        else
//            state = 2;
//      }
//
//      else if(TE_FIL < TE_LIM_LOW)
//      {
//        state = 0;
//        TI_CROSS = 0;
//      }
//
//      sendCommand(pkt.senderId,state);
//
//      Serial.print("STATE SENT: ");
//      Serial.println(state);
//    }
//  }
//}






#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define EDGE_ID 0
#define NODE1_ID 1
#define NODE3_ID 3

#define TYPE_POLL 0
#define TYPE_DATA 1
#define TYPE_COMMAND 2

RF24 radio(9,8);

const byte ADDR_EDGE[] = "NODE2";
const byte ADDR_NODE1[] = "NODE1";
const byte ADDR_NODE3[] = "NODE3";

struct Packet{
  uint8_t senderId;
  uint8_t type;
  int value;
};

float TE_LIM_HIGH = 40.0;
float TE_LIM_LOW  = 38.0;

float TE_FIL = 0;

unsigned long TI_CROSS = 0;
unsigned long TI_TH = 5000;

unsigned long lastPoll = 0;
int currentNode = NODE1_ID;

void setup()
{
  Serial.begin(9600);   // UART to ESP8266

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(Packet));
  radio.setAutoAck(false);

  radio.openReadingPipe(1,ADDR_EDGE);
  radio.startListening();

  Serial.println("EDGE_READY");
}

void pollNode(const byte* addr)
{
  Packet poll;

  poll.senderId = EDGE_ID;
  poll.type = TYPE_POLL;
  poll.value = 0;

  radio.stopListening();
  radio.openWritingPipe(addr);
  radio.write(&poll,sizeof(poll));
  radio.startListening();
}

void sendCommand(uint8_t node,int state)
{
  Packet cmd;

  cmd.senderId = EDGE_ID;
  cmd.type = TYPE_COMMAND;
  cmd.value = state;

  radio.stopListening();

  if(node == NODE1_ID)
      radio.openWritingPipe(ADDR_NODE1);
  else
      radio.openWritingPipe(ADDR_NODE3);

  radio.write(&cmd,sizeof(cmd));
  radio.startListening();
}

void loop()
{
  unsigned long now = millis();

  // Poll nodes
  if(now - lastPoll > 2000)
  {
    lastPoll = now;

    if(currentNode == NODE1_ID)
    {
      pollNode(ADDR_NODE1);
      currentNode = NODE3_ID;
    }
    else
    {
      pollNode(ADDR_NODE3);
      currentNode = NODE1_ID;
    }
  }

  if(radio.available())
  {
    Packet pkt;
    radio.read(&pkt,sizeof(pkt));

    if(pkt.type == TYPE_DATA)
    {
      float temp = pkt.value / 10.0;

      Serial.print("EDGE,");
      Serial.print(pkt.senderId);
      Serial.print(",");
      Serial.print(temp);
      Serial.print(",");
      // send to ESP8266

      TE_FIL = 0.7 * TE_FIL + 0.3 * temp;

      int state = 0;

      if(TE_FIL > TE_LIM_HIGH)
      {
        if(TI_CROSS == 0)
            TI_CROSS = now;

        unsigned long dur = now - TI_CROSS;

        if(dur < TI_TH)
            state = 1;
        else
            state = 2;
      }

      else if(TE_FIL < TE_LIM_LOW)
      {
        state = 0;
        TI_CROSS = 0;
      }

      sendCommand(pkt.senderId,state);
      Serial.println(state);
    }
  }
}
