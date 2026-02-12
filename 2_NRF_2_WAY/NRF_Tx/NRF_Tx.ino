//BASIC NRF Tx

//#include <SPI.h>
//#include <RF24.h>
//#include <nRF24L01.h>
//
//RF24 radio(9, 8);   // CE=9, CSN=8
//
//const byte NODE2_ADDR[] = "NODE2";   // ✅ renamed
//
//int number = 1;
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
//  radio.openWritingPipe(NODE2_ADDR);
//  radio.stopListening();
//
//  Serial.println("NODE1 TX READY");
//}
//
//void loop() {
//  radio.write(&number, sizeof(number));
//
//  Serial.print("SENT: ");
//  Serial.println(number);
//
//  number++;
//  if (number > 99) number = 1;
//
//  delay(500);
//}






////////////////////////////////////////////////////////////////////////////////////////////Tx with recieve///////////////////////////////////////////////
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(9, 8);

const byte ADDR_NODE1[] = "NODE1";
const byte ADDR_NODE2[] = "NODE2";

int txValue = 1;

const unsigned long PERIOD = 2000;   // total frame (ms)
const unsigned long SLOT   = 1000;   // slot length (ms)

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);

  radio.disableDynamicPayloads();
  radio.setPayloadSize(sizeof(int));
  radio.setAutoAck(false);

  Serial.println("NODE1 READY");
}

void loop() {
  unsigned long slotTime = millis() % PERIOD;

  // -------- SLOT A : NODE1 → NODE2 --------
  if (slotTime < SLOT) {
    radio.stopListening();
    radio.openWritingPipe(ADDR_NODE2);

    radio.write(&txValue, sizeof(txValue));
    Serial.print("NODE1 SENT: ");
    Serial.println(txValue);

    txValue++;
    delay(200);
  }

  // -------- SLOT B : NODE1 ← NODE2 --------
  else {
    radio.openReadingPipe(1, ADDR_NODE1);
    radio.startListening();

    if (radio.available()) {
      int rx;
      radio.read(&rx, sizeof(rx));
      Serial.print("NODE1 RECEIVED: ");
      Serial.println(rx);
    }
  }
}
