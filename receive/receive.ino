#include <EEPROM.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

#define MaxNum 20

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
  Serial.begin(115200);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();

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
  Serial.println("电磁阀数据读取：");
  Serial.println("**************************");
  Serial.println("电磁铁序号ID及所对应的引脚和引脚电平为：");

  for (int x = 0; x < MaxNum; x++)
  {
    Serial.print(ELMA.ID[x]);
    Serial.print("*");
    Serial.print(ELMA.pin[x]);
    Serial.print("*");
    Serial.print(ELMA.state[x]);
    Serial.print("；");
  }
  Serial.print("\n");
  Serial.println("电磁铁连接方式为：");
  for (int y = 0; y < MaxNum; y++)
  {
    Serial.print(ELMA.FA[y]);
    Serial.print("*");
  }
  Serial.print("\n");
  Serial.println("**************************");
}

void loop()
{
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
  {
    Serial.println();
    Serial.print("接收到的数据为：");
    for (int i = 0; i < canMsg.can_dlc; i++)
    { // print the data
      Serial.print("0x");
      Serial.print(canMsg.data[i], HEX);
      Serial.print("*");
    }
    Serial.println();
    Serial.println("**************************");
    /*如果第一个字节值不为0，则逐个读取接下来6个字节的值，并从最后一个字节中读取状态值；
      如果第一个字节值为0，则读取接下来三个字节的数存储在data.data数组中*/
    if (canMsg.data[0] != 0)
    {
      //将最后一个字节的数转换为二进制数保存在data.data1[8]数组中
      int ix, cx = 0, a[8];
      while (canMsg.data[7] != 0)
      {
        ix = canMsg.data[7] % 2;
        a[cx] = ix;
        cx++;
        canMsg.data[7] = canMsg.data[7] / 2;
      }
      cx--;
      for (; cx >= 0; cx--)
        data.data1[7 - cx] = a[cx];


      //逐个读取第2-6个字节的值
      for (int i = 1; i < 7; i++)
      {
        if (canMsg.data[i] != 0)  //buf[i]不为0则继续；为0则break中断此次循环，开始下一循环
        {
          for (int y = 0; y < MaxNum; y++)
          {
            if (ELMA.ID[y] == canMsg.data[i])   //判断buf[i]是在描述哪个电磁阀上的电磁铁
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
                          Serial.print("电磁阀ID：");
                          Serial.print(ELMA.ID[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[y]);
                          Serial.print(" 与");
                          Serial.print("电磁阀ID：");
                          Serial.print(ELMA.ID[b]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[b]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[b]);
                          Serial.print("；");
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
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[y]);
                          Serial.println("；");
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
                          Serial.print("电磁阀ID：");
                          Serial.print(ELMA.ID[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[y]);
                          Serial.print(" 与");
                          Serial.print("电磁阀ID：");
                          Serial.print(ELMA.ID[c]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[c]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[c]);
                          Serial.print("；");
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
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚：");
                          Serial.print(ELMA.pin[y]);
                          Serial.print("；");
                          Serial.print("电磁阀所对应的引脚状态：");
                          Serial.print(ELMA.state[y]);
                          Serial.println("；");
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
      int ix, cx = 0, dx = 0, ex = 0, a[8];
      while (canMsg.data[1] != 0)
      {
        ix = canMsg.data[1] % 2;
        a[cx] = ix;
        cx++;
        canMsg.data[1] = canMsg.data[1] / 2;
      }
      cx--;
      for (; cx >= 0; cx--)
        data.data1[7 - cx] = a[cx];

      while (canMsg.data[2] != 0)
      {
        ix = canMsg.data[2] % 2;
        a[dx] = ix;
        dx++;
        canMsg.data[2] = canMsg.data[2] / 2;
      }
      dx--;
      for (; dx >= 0; dx--)
        data.data2[7 - dx] = a[dx];

      while (canMsg.data[3] != 0)
      {
        ix = canMsg.data[3] % 2;
        a[ex] = ix;
        ex++;
        canMsg.data[3] = canMsg.data[3] / 2;
      }
      ex--;
      for (; ex >= 0; ex--)
        data.data3[7 - ex] = a[ex];
      Serial.println();
      Serial.print("20个电磁铁状态为：");
      //将20个引脚状态写入state数组
      for (int x = 0; x < MaxNum; x++)
      {
        if ((x >= 0) && (x < 8))
        {
          ELMA.state[x] = data.data1[x];
          Serial.print(ELMA.state[x]);
          Serial.print("；");
        }
        else if ((x >= 8) && (x < 16))
        {
          ELMA.state[x] = data.data2[x - 8];
          Serial.print(ELMA.state[x]);
          Serial.print("；");
        }
        else
        {
          ELMA.state[x] = data.data3[x - 16];
          Serial.print(ELMA.state[x]);
          Serial.print("；");
        }
      }
      Serial.println();
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
                    Serial.print("电磁阀ID：");
                    Serial.print(ELMA.ID[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[y]);
                    Serial.print(" 与");
                    Serial.print("电磁阀ID：");
                    Serial.print(ELMA.ID[b]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[b]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[b]);
                    Serial.print("；");
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
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[y]);
                    Serial.println("；");
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
                    Serial.print("电磁阀ID：");
                    Serial.print(ELMA.ID[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[y]);
                    Serial.print(" 与");
                    Serial.print("电磁阀ID：");
                    Serial.print(ELMA.ID[c]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[c]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[c]);
                    Serial.print("；");
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
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚：");
                    Serial.print(ELMA.pin[y]);
                    Serial.print("；");
                    Serial.print("电磁阀所对应的引脚状态：");
                    Serial.print(ELMA.state[y]);
                    Serial.println("；");
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
