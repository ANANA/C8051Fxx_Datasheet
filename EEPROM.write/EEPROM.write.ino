#include <SPI.h>
#include "mcp_can.h"
#include<EEPROM.h>

#define MaxNum 20
struct electromagnet {
  int ID[MaxNum] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; //电磁铁序号
  int pin[MaxNum] = {3, 4, 5, 6, 7, 8, 9, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34}; //每个电磁铁所对应的Arduino上引脚位置
  int FA[MaxNum] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; //同一个阀上两个电磁铁的连接方式
} ELMA;
/*   example
  ELMA.pin[MaxNum]={3,4,5,6,7,8,9,22,23,24,25,26,27,28,29,30,31,32,33,34};
  ELMA.pin[0] = 3;  //电磁铁1所对应的引脚
  ELMA.pin[1] = 4; //电磁铁2所对应的引脚
  ELMA.pin[2] = 5; //电磁铁3所对应的引脚
  ELMA.pin[3] = 6; //电磁铁4所对应的引脚
  ELMA.pin[4] = 7; //电磁铁5所对应的引脚
  ELMA.pin[5] = 8; //电磁铁6所对应的引脚
  ELMA.pin[6] = 9; //电磁铁7所对应的引脚
  ELMA.pin[7] = 22; //电磁铁8所对应的引脚
  ELMA.pin[8] = 23; //电磁铁9所对应的引脚
  ELMA.pin[9] = 24; //电磁铁10所对应的引脚
  ELMA.pin[10] = 25; //电磁铁11所对应的引脚
  ELMA.pin[11] = 26; //电磁铁12所对应的引脚
  ELMA.pin[12] = 27; //电磁铁13所对应的引脚
  ELMA.pin[13] = 28; //电磁铁14所对应的引脚
  ELMA.pin[14] = 29; //电磁铁15所对应的引脚
  ELMA.pin[15] = 30; //电磁铁16所对应的引脚
  ELMA.pin[16] = 31; //电磁铁17所对应的引脚
  ELMA.pin[17] = 32; //电磁铁18所对应的引脚
  ELMA.pin[18] = 33; //电磁铁19所对应的引脚
  ELMA.pin[19] = 34; //电磁铁20所对应的引脚  */
/*example
  ELMA.FA[MaxNum]={1,7,10,4,5,9,2,8,6,3,13,19,11,18,15,16,17,14,12,20};
  ELMA.FA[0] = 1;
  ELMA.FA[1] = 7;     //阀1的两个电磁铁对应的ID
  ELMA.FA[2] = 10;
  ELMA.FA[3] = 4;     //阀2的两个电磁铁对应的ID
  ELMA.FA[4] = 5;
  ELMA.FA[5] = 9;     //阀3的两个电磁铁对应的ID
  ELMA.FA[6] = 2;
  ELMA.FA[7] = 8;     //阀4的两个电磁铁对应的ID
  ELMA.FA[8] = 6;
  ELMA.FA[9] = 3;     //阀5的两个电磁铁对应的ID
  ELMA.FA[10] = 13;
  ELMA.FA[11] = 19;     //阀6的两个电磁铁对应的ID
  ELMA.FA[12] = 11;
  ELMA.FA[13] = 18;     //阀7的两个电磁铁对应的ID
  ELMA.FA[14] = 15;
  ELMA.FA[15] = 16;     //阀8的两个电磁铁对应的ID
  ELMA.FA[16] = 17;
  ELMA.FA[17] = 14;     //阀9的两个电磁铁对应的ID
  ELMA.FA[18] = 12;
  ELMA.FA[19] = 20;     //阀10的两个电磁铁对应的ID   */

int addr = 0;


void setup() {
  Serial.begin(115200);
  /*
    for(;addr<20;addr++)
    EEPROM.write(addr,ELMA.ID[addr]);
    for(addr=20;addr<40;addr++)
    EEPROM.write(addr,ELMA.pin[addr-20]);
    for(addr=40;addr<60;addr++)
    EEPROM.write(addr,ELMA.FA[addr-40]);
  */

  for (int x = 0; x < MaxNum; x++)
  {
    EEPROM.write(addr, ELMA.ID[x]);
    addr += 1;
    EEPROM.write(addr, ELMA.pin[x]);
    addr += 1;
    EEPROM.write(addr, ELMA.FA[x]);
    addr += 1;
  }

  Serial.print("电磁阀数据写入：");
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

}
void loop() {

}

