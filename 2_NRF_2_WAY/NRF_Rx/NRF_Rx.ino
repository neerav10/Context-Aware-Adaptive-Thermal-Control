//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);   // CE=9, CSN=8
//
//const byte NODE2_ADDR[] = "NODE2";   // ✅ same name, OK
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
//  radio.openReadingPipe(1, NODE2_ADDR);
//  radio.startListening();
//
//  Serial.println("NODE2 RX READY");
//}
//
//void loop() {
//  if (radio.available()) {
//    int rx;
//    radio.read(&rx, sizeof(rx));
//
//    Serial.print("RECEIVED: ");
//    Serial.println(rx);
//  }
//}

//-------------------------------------Rx with send---------------------
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(9, 8);

const byte ADDR_NODE2[] = "NODE2";
const byte ADDR_NODE1[] = "NODE1";

unsigned long frameStart = 0;
int lastRx = 0;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);

  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(int));
  radio.setAutoAck(false);

  frameStart = millis();
  Serial.println("NODE2 READY");
}

void loop() {
  unsigned long now = millis();
  unsigned long slot = (now - frameStart) % 2000;

  // SLOT A: NODE2 ← NODE1
  if (slot < 1000) {
    radio.openReadingPipe(1, ADDR_NODE2);
    radio.startListening();

    if (radio.available()) {
      radio.read(&lastRx, sizeof(lastRx));
      Serial.print("NODE2 RECEIVED: ");
      Serial.println(lastRx);
    }
  }

  // SLOT B: NODE2 → NODE1
  else {
    radio.stopListening();
    radio.openWritingPipe(ADDR_NODE1);

    int reply = lastRx;   // 🔥 ECHO BACK SAME VALUE
    radio.write(&reply, sizeof(reply));

    Serial.print("NODE2 SENT: ");
    Serial.println(reply);
    delay(200);
  }
}
