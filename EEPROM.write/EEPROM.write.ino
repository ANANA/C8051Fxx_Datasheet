#include <SPI.h>
#include "mcp_can.h"
#include<EEPROM.h>

#define MaxNum 20
struct electromagnet {
        char ID[MaxNum];     //电磁铁序号
        char pin[MaxNum];   //每个电磁铁所对应的Arduino上引脚位置
}ELMA;


int addr = 0;
 
  
void setup() {  
  Serial.begin(115200); 

int index=1;
for(int i=0;i<MaxNum;i++)
{
  ELMA.ID[i]=index;
  index+=1;
}

             //example
ELMA.pin[0]=3;    //电磁铁1所对应的引脚   //三位两通电磁换向阀
ELMA.pin[1]=4;   //电磁铁2所对应的引脚
ELMA.pin[2]=5;  //电磁铁3所对应的引脚
ELMA.pin[3]=6;   //电磁铁4所对应的引脚
ELMA.pin[4]=7;   //电磁铁5所对应的引脚
ELMA.pin[5]=8;  //电磁铁6所对应的引脚
ELMA.pin[6]=9;   //电磁铁7所对应的引脚
ELMA.pin[7]=22;   //电磁铁8所对应的引脚
ELMA.pin[8]=23;  //电磁铁9所对应的引脚
ELMA.pin[9]=24;  //电磁铁10所对应的引脚
ELMA.pin[10]=25;   //电磁铁11所对应的引脚
ELMA.pin[11]=26;   //电磁铁12所对应的引脚
ELMA.pin[12]=27;   //电磁铁13所对应的引脚
ELMA.pin[13]=28;   //电磁铁14所对应的引脚
ELMA.pin[14]=29;   //电磁铁15所对应的引脚
ELMA.pin[15]=30;   //电磁铁16所对应的引脚
ELMA.pin[16]=31;   //电磁铁17所对应的引脚
ELMA.pin[17]=32;   //电磁铁18所对应的引脚
ELMA.pin[18]=33;   //电磁铁19所对应的引脚
ELMA.pin[19]=34;   //电磁铁20所对应的引脚

}  
void loop() {  
  for(int x=0; x<MaxNum;x++)
  {
  EEPROM.write(addr, ELMA.ID[x]);
  addr+=1;
  EEPROM.write(addr, ELMA.pin[x]);
  addr+=1;
  }
}

