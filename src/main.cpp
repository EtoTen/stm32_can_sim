/*
------------
Speed message, so that Sync does not think that the vehicle is moving
----

https://www.explorerforum.com/forums/threads/anybody-play-with-arduino-can-bus-looking-for-the-front-camera-trigger.482116/?amp=1

"ID: 109, Data: 00 03 11 00 00 00 28 0
00 03: Current RPM, multiplied by 4, in hex. So a value of 1B 4F in hex is 6991 in decimal, divided by 4 is 1,747 RPM
11: Current gear. 01 is Park, 11 is Reverse, 21 is Neutral, 31 is Drive, and 41 is Sport/Low/Manual/Whatever.
00 00 00: Current speed, possibly multiplied by 2, in hex. Car wasn't moving so it's all zeros.
28: Ignition mode. 28 is accessory, 48 is cranking, 58 is engine running.""

------------
Turn on message, so that Sync turns on
----
https://ffclub.ru/topic/444743/jump_330/

"
1. Подключаемся с I-CAN синка через HS-CAN elm327:
19 pin sync <--> 6 pin elm327
20 pin sync <--> 14 pin elm327
Не забываем про резистор 120Ом между линиями (без него мой синк не стартовал)

2.Берем программку для отправки CAN сообщений и шлем пакет чтобы разбудить синк:
цитата:
048 00 00 00 00 07 00 E0 00
или
цитата:
3B3 00 00 00 00 07 00 e0 00
или
цитата:
3B3 41 00 00 00 4С 00 00 00"
*/

#include <arduino.h>
#include <eXoCAN.h>
#define bluePillLED PC13

struct can_messages {
 int can_id;
 uint8_t can_message[8];
};

static can_messages cMessages[3] = { 
  {{0x048},{0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0xE0, 0x00}},  // turn on C1MCA - this turned most units on
 // {{0x3B3},{0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0xE0, 0x00}},  // turn on CGEA 1.3 - newer protocol, this actually made some APIMs turn off so it is commented out
  {{0x3B3},{0x41, 0x00, 0x00, 0x00, 0x4c, 0x00, 0xE0, 0x00}},  // turn CGEA 1.3 - second option, seems to work better
  {{0x109},{0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x28}}   // Accessory ON, Gear Park, Speed 0 - this needed or else the APIM will default to driving mode and will lock out many settings
  };

uint8_t txDataLen = 8;
uint32_t txDly = 100; // mSec

// 11b IDs, 500k bit rate, tja1040, portA pins 11,12
eXoCAN can(STD_ID_LEN, BR500K, PORTA_11_12_XCVR); 

void setup()
{ 
  pinMode(bluePillLED, OUTPUT);
}

uint32_t last = 0;
int cMSG = 0;

void loop()
{    
  
  if (millis() / txDly != last)     //executes every txDly ms  
  {  
    last = millis() / txDly;

    can.transmit(cMessages[cMSG].can_id, cMessages[cMSG].can_message, txDataLen);  
   
    if (cMSG<(sizeof(cMessages) / sizeof(cMessages[0]))) {
      cMSG++;          
    }     
    else {       
      cMSG=0;    
    }
    digitalToggle(bluePillLED); //blinky lights are cool
  }
 }


