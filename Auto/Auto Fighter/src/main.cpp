#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


//Radio Setup
RF24 radio(7, 8);  // CE, CSN
byte sAddresses[][6] = {"BasT","BasR"};

const bool autoPair = true;

//Radio Pair
typedef struct{
  int id = 20; // Each student will receive a id number
  bool paired = false;
}
pair;
pair pairData;


//Controller Data
typedef struct{
  int leftSpeed = 10;   // debug value
  int rightSpeed = 90;
  bool leftTrigger = 0;
  bool rightTrigger = 0;

}
ctrl;
ctrl ctrlData;

void pairNow(){  
  radio.begin();
  radio.setChannel(1);
  // radio.openReadingPipe(1, sAddresses[0]);
  radio.openWritingPipe(sAddresses[0]);  
  radio.stopListening();
  // radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  // radio.startListening();

  bool rslt;
  rslt = radio.write( &pairData, sizeof(pairData) );

  Serial.print("Data Sent Id = ");
  Serial.print(pairData.id);

  if (rslt) {
        Serial.println("  Acknowledge received");
        pairData.paired = true;
        radio.setChannel(pairData.id);
        radio.stopListening();
        radio.openReadingPipe(1, sAddresses[0]);
        radio.startListening();
    }
    else {
        Serial.println("  Tx failed");
    }

}

void setup() {
  Serial.begin(9600);
  Serial.println("Fighter Init");

  if (!autoPair)
  {
    radio.begin();
    radio.setChannel(pairData.id);
    radio.openReadingPipe(1, sAddresses[0]);
    radio.startListening();
    Serial.println("Manual Pair");
    pairData.paired = true;
  } else{
    Serial.println("Auto Pair");
  } 

  //radio.setPALevel(RF24_PA_LOW); //Default Max Power

}

void loop() {
  if (!pairData.paired)
  {
    pairNow();
  } else {
    if (radio.available())
    {
      // Serial.println("Received");
      radio.read(&ctrlData, sizeof(ctrlData));
      Serial.println(ctrlData.leftSpeed);
    }

  }

}
