#include <SPI.h>
#include "mcp_can.h"
#include<EEPROM.h>

#define MaxNum 20
struct electromagnet {
  int ID[MaxNum];     //电磁铁序号
  int pin[MaxNum];   //每个电磁铁所对应的Arduino上引脚位置
  int FA[MaxNum];   //同一个阀上两个电磁铁的连接方式
} ELMA;

int addr = 0;

void setup() {

  Serial.begin(115200);
  /*
    for(;addr<20;addr++)
    ELMA.ID[addr]=EEPROM.read(addr);
    for(addr=20;addr<40;addr++)
    ELMA.pin[addr-20]=EEPROM.read(addr);
    for(addr=40;addr<60;addr++)
    ELMA.FA[addr-40]=EEPROM.read(addr);
  */

  for (int i = 0; i < MaxNum; i++)
  {
    ELMA.ID[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.pin[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.FA[i] = EEPROM.read(addr);
    addr += 1;
  }

  Serial.print("电磁阀数据读取：");
  Serial.print("\n");
  Serial.print("**************************");
  Serial.print("\n");

  Serial.print("电磁铁序号ID及所对应的引脚为：");
  Serial.print("\n");
  for (int x = 0; x < MaxNum; x++)
  {
    Serial.print(ELMA.ID[x]);
    Serial.print("*");
    Serial.print(ELMA.pin[x]);
    Serial.print("；");
  }
  Serial.print("\n");
  Serial.print("电磁铁连接方式为：");
  Serial.print("\n");
  for (int y = 0; y < MaxNum; y++)
  {
    Serial.print(ELMA.FA[y]);
    Serial.print("*");
  }
  Serial.print("\n");
  Serial.print("**************************");

  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}


void loop() {


}
