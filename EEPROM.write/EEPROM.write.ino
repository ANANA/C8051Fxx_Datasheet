#include <SPI.h>
#include<EEPROM.h>

#define MaxNum 20
struct electromagnet {
  int ID[MaxNum] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; //电磁铁序号
  int pin[MaxNum] = {3, 4, 5, 6, 7, 8, 9, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34}; //每个电磁铁所对应的Arduino上引脚位置
  int FAD[MaxNum] = {1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //单控电磁阀电磁铁ID
  int FAS[MaxNum] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0}; //双控电磁阀同一个阀上两个电磁铁的ID
  int state[MaxNum];//每个电磁铁所对应引脚状态
} ELMA;

int addr = 0;
void setup() {
  Serial.begin(115200);
  Serial.println("电磁阀数据开始写入：");
  Serial.println("**************************");
  Serial.println();
  for (int x = 0; x < MaxNum; x++)
  {
    EEPROM.write(addr, ELMA.ID[x]);
    addr += 1;
    EEPROM.write(addr, ELMA.pin[x]);
    addr += 1;
    EEPROM.write(addr, ELMA.FAD[x]);
    addr += 1;
    EEPROM.write(addr, ELMA.FAS[x]);
    addr += 1;
    Serial.println("电磁阀数据写入中- - - - -");
  }

  /*
    Serial.println("**************************");
    Serial.println("电磁铁序号ID及所对应的引脚为：");

    for (int x = 0; x < MaxNum; x++)
    {
    Serial.print(ELMA.ID[x]);
    Serial.print("*");
    Serial.print(ELMA.pin[x]);
    Serial.print("；");
    }
    Serial.print("\n");
    Serial.println("单控电磁阀引脚为");
    for (int y = 0; y < MaxNum; y++)
    {
    Serial.print(ELMA.FA[y]);
    Serial.print("*");
    }
    Serial.print("\n");
  */
  Serial.println();
  Serial.println("电磁阀数据写入成功！");
  Serial.println("**************************");

}
void loop() {

}

