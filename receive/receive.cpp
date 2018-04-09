#include <SPI.h>
#include "mcp_can.h"
#include <EEPROM.h>

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);                  // 设置 CS 引脚

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];

#define MaxNum 20
#define N 8
#define YM 01//掩码
/*位与&：查看一个位
  位移>>：向左移动位*/

struct electromagnet
{
  int ID[MaxNum];     //电磁铁序号
  int pin[MaxNum];   //每个电磁铁所对应的Arduino上引脚位置
  int FA[MaxNum];    //阀的连接方式
  int state[MaxNum];//每个电磁铁所对应引脚状态
} ELMA;

struct DATA
{
  int data1[8];
  int data2[8];
  int data3[8];
  int data4[8];
} data;

void setup()
{
  Serial.begin(115200);         //baudrate=500k

  //初始化读取EEPROM中信息并初始化 引脚状态
  int addr = 0;
  for (int i = 0; i < MaxNum; i++)
  {
    ELMA.ID[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.pin[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.FA[i] = EEPROM.read(addr);
    addr += 1;
    pinMode(ELMA.pin[i], OUTPUT);
    ELMA.state[i] = 1;   //1表示断电；0表示通电
    digitalWrite(ELMA.pin[i], HIGH);
  }
  //测试用
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

  //CAN 初始化
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // 初始化CAN硬件 : baudrate = 500k
  {
    Serial.println("CAN BUS Shield 初始化失败！");
    Serial.println(" 请重新初始化 CAN BUS Shield ！");
    delay(100);
  }
  Serial.println("CAN BUS Shield 初始化成功!");

  // 监视中断输入引脚的变化
  attachInterrupt(0, MCP2515_ISR, FALLING);    //外部中断，当int.0（D2引脚）电平改变时,触发中断函数MCP2515_ISR      FALLING-下降沿触发

}

void MCP2515_ISR()   //中断函数
{
  flagRecv = 1;
}

void loop()
{
  if (flagRecv)
  { // 检查是否有数据来到
    flagRecv = 0;                   // 清除中断标识符

    while (CAN_MSGAVAIL == CAN.checkReceive())
    {
      // 读取数据  len: data length, buf: data buf
      CAN.readMsgBuf(&len, buf);
    }

    /*如果第一个字节值不为0，则逐个读取接下来6个字节的值，并从最后一个字节中读取状态值；
      如果第一个字节值为0，则读取接下来三个字节的数存储在data.data数组中*/
    if (buf[0] != 0)
    {
      //将最后一个字节的数转换为二进制数保存在data.data1[8]数组中
      int a = N;
      while (a--)
      {
        data.data1[a] = YM & buf[7] >> a;
      }

      //逐个读取第2-6个字节的值
      for (int i = 1; i < 7; i++)
      {
        if (buf[i] != 0)  //buf[i]不为0则继续；为0则break中断此次循环，开始下一循环
        {
          for (int y = 0; y < MaxNum; y++)
          {
            if (ELMA.ID[y] == buf[i])   //判断buf[i]是在描述哪个电磁阀上的电磁铁
            {
              ELMA.state[y] = data.data1[i];

              //判断是否存在冲突
              //判断电磁铁在哪个电磁阀上，并判断是否电磁阀上两个电磁铁是否同时接通，是则报错，并结束，否则将状态发送到对应的引脚
              for (int z = 0; z < MaxNum; z++)
              {
                if (ELMA.FA[z] == ELMA.ID[y])
                {
                  if ((z % 2) != 0) //z位奇数，但在偶数位
                  {
                    for (int b = 0; b < MaxNum; b++)
                    {
                      if (ELMA.ID[b] == ELMA.FA[z - 1])
                      {
                        if ((ELMA.state[b] == 0) && (ELMA.state[y] == 0))
                        {
                          Serial.println("错误！阀的两端同时通电！");
                          break;
                        }
                        else
                        {
                          //不发生冲突时才会将状态发送到对应的引脚
                          if (ELMA.state[y] == 1)
                            pinMode(ELMA.pin[y], HIGH);
                          else
                            pinMode(ELMA.pin[y], LOW);
                            //测试用
                            Serial.print("电磁阀ID：");
                            Serial.print(ELMA.ID[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚：");
                            Serial.print(ELMA.pin[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚状态：");
                            Serial.print(ELMA.state[y]);
                            Serial.print(";");
                            Serial.print("\n");
                          
                        }
                        break;
                      }

                    }
                  }
                  else  //z为偶数，但在奇数位
                  {
                    for (int c = 0; c < MaxNum; c++)
                    {
                      if (ELMA.ID[c] == ELMA.FA[z + 1])
                      {
                        if ((ELMA.state[c] == 0) && (ELMA.state[y] == 0))
                        {
                          Serial.println("错误！阀的两端同时通电！");
                          break;
                        }
                        else
                        {
                          //不发生冲突时才会将状态发送到对应的引脚
                          if (ELMA.state[y] == 1)
                            pinMode(ELMA.pin[y], HIGH);
                          else
                            pinMode(ELMA.pin[y], LOW);
                          //测试用
                            Serial.print("电磁阀ID：");
                            Serial.print(ELMA.ID[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚：");
                            Serial.print(ELMA.pin[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚状态：");
                            Serial.print(ELMA.state[y]);
                            Serial.print(";");
                            Serial.print("\n");
                         
                        }
                        break;
                      }
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
    }
    else
    {
      int a = N;
      while (a--)
      {
        data.data1[a] = YM & buf[1] >> a;
        data.data2[a] = YM & buf[2] >> a;
        data.data3[a] = YM & buf[3] >> a;
      }//将十进制转换为二进制存储在data.data数组中

      //将20个引脚状态写入state数组
      for (int x = 0; x < MaxNum; x++)
      {
        if ((x >= 0) && (x < 8))
          ELMA.state[x] = data.data2[x];
        else if ((x >= 8) && (x < 16))
          ELMA.state[x] = data.data3[x - 8];
        else
          ELMA.state[x] = data.data4[x - 16];
      }

      for (int y = 0; y < MaxNum; y++)
      {
        //判断电磁铁在哪个电磁阀上，并判断是否电磁阀上两个电磁铁是否同时接通，是则报错，并结束
        for (int z = 0; z < MaxNum; z++)
        {
          if (ELMA.FA[z] == ELMA.ID[y])
          {
            if ((z % 2) != 0) //z位奇数，但在偶数位
            {
              for (int b = 0; b < MaxNum; b++)
              {
                if (ELMA.ID[b] == ELMA.FA[z - 1])
                {
                  if ((ELMA.state[b] == 0) && (ELMA.state[y] == 0))
                  {
                    Serial.println("错误！阀的两端同时通电！");
                    break;
                  }
                  else
                  {
                    //不发生冲突时才会将状态发送到对应的引脚
                    if (ELMA.state[y] == 1)
                      pinMode(ELMA.pin[y], HIGH);
                    else
                      pinMode(ELMA.pin[y], LOW);
                    //测试用
                            Serial.print("电磁阀ID：");
                            Serial.print(ELMA.ID[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚：");
                            Serial.print(ELMA.pin[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚状态：");
                            Serial.print(ELMA.state[y]);
                            Serial.print(";");
                            Serial.print("\n");
                        
                  }
                  break;
                }
              }
            }
            else  //z为偶数，但在奇数位
            {
              for (int c = 0; c < MaxNum; c++)
              {
                if (ELMA.ID[c] == ELMA.FA[z + 1])
                {
                  if ((ELMA.state[c] == 0) && (ELMA.state[y] == 0))
                  {
                    Serial.println("错误！阀的两端同时通电！");
                    break;
                  }
                  else
                  {
                    //不发生冲突时才会将状态发送到对应的引脚
                    if (ELMA.state[y] == 1)
                      pinMode(ELMA.pin[y], HIGH);
                    else
                      pinMode(ELMA.pin[y], LOW);
                  //测试用
                            Serial.print("电磁阀ID：");
                            Serial.print(ELMA.ID[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚：");
                            Serial.print(ELMA.pin[y]);
                            Serial.print(";");
                            Serial.print("电磁阀所对应的引脚状态：");
                            Serial.print(ELMA.state[y]);
                            Serial.print(";");
                            Serial.print("\n");
                       
                  }
                  break;
                }
              }
            }
            break;
          }
        }
      }
    }
  }
}