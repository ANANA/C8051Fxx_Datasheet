#include <EEPROM.h>
#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

#define MaxNum 20
#define ON 1  //引脚高电平，电磁铁通电运行
#define OFF 0 //引脚低电平，电磁铁断电不运行

struct electromagnet
{
  int ID[MaxNum];     //电磁铁序号
  int pin[MaxNum];    //每个电磁铁所对应的Arduino上引脚位置
  int FAD[MaxNum];    //单控电磁阀电磁铁ID
  int FAS[MaxNum];    //双控电磁阀同一个阀上两个电磁铁的ID
  int state[MaxNum];  //每个电磁铁所对应引脚状态
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

  Serial.println();
  Serial.println("CAN初始化完成！");
  Serial.println();

  //初始化读取EEPROM中信息并初始化 引脚状态
  Serial.println("开始读取EEPROM中电磁阀数据：");
  Serial.println("**************************");
  Serial.println();
  int addr = 0;
  for (int i = 0; i < MaxNum; i++)
  {
    ELMA.ID[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.pin[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.FAD[i] = EEPROM.read(addr);
    addr += 1;
    ELMA.FAS[i] = EEPROM.read(addr);
    addr += 1;
    pinMode(ELMA.pin[i], OUTPUT);
    ELMA.state[i] = OFF;
    digitalWrite(ELMA.pin[i], OFF);
    Serial.println("电磁阀数据读取中- - - - -");
  }
  /*
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
  */

  Serial.print("\n");
  Serial.println("电磁阀数据读取完成！");
  Serial.println("Arduino引脚初始化完成！");
  Serial.println("**************************");
  Serial.println();
  Serial.println();
}

void loop()
{
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
  {
    Serial.println();
    Serial.print("成功接收到数据！");
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
      int ix, cxX = 0, a[8];
      while (canMsg.data[7] != 0)
      {
        ix = canMsg.data[7] % 2;
        a[cxX] = ix;
        cxX++;
        canMsg.data[7] = canMsg.data[7] / 2;
      }
      cxX--;
      for (; cxX >= 0; cxX--)
        data.data1[7 - cxX] = a[cxX];


      //逐个读取第2-6个字节的值
      for (int i = 1; i < 7; i++)
      {
        if (canMsg.data[i] != 0)  //buf[i]不为0则继续；为0则break中断此次循环，开始下一循环
        {
          for (int y = 0; y < MaxNum; y++)
          {
            if (ELMA.ID[y] == canMsg.data[i])   //判断buf[i]是在描述哪个电磁阀上的电磁铁
            {
              ELMA.state[y] = data.data1[i - 1];

              //判断是否存在冲突
              //判断电磁铁在哪个电磁阀上，并判断是否电磁阀上两个电磁铁是否同时接通，是则报错，并结束，否则将状态发送到对应的引脚

              for (int ax = 0; ax < MaxNum; ax++) //判断是单控电磁阀还是双控电磁阀
              {
                if (ELMA.FAS[ax] == ELMA.ID[y]) //为双控电磁阀，执行如下程序
                {
                  if ((ax % 2) != 0) //ax位奇数，但在偶数位
                  {
                    for (int bx = 0; bx < MaxNum; bx++)
                    {
                      if (ELMA.ID[bx] == ELMA.FAS[ax - 1])
                      {
                        //判断同一电磁阀两端是否同时通电
                        if ((ELMA.state[bx] == ON) && (ELMA.state[y] == ON))  //同时通电，报错并返回
                        {
                          Serial.print("错误！");
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[y]);
                          Serial.print(" 与");
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[bx]);
                          Serial.println("同时通电！");
                          Serial.println("引脚状态写入失败！");
                          break;
                        }
                        else  //没有同时通电，继续运行
                        {
                          //不发生冲突时才会将状态发送到对应的引脚
                          if (ELMA.state[y] == 1)
                            pinMode(ELMA.pin[y], HIGH);
                          else
                            pinMode(ELMA.pin[y], LOW);
                          //选择性用
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[y]);
                          Serial.println("引脚状态写入成功！");
                        }
                        break;
                      }
                    }
                  }
                  else  //y为偶数，但在奇数位
                  {
                    for (int cx = 0; cx < MaxNum; cx++)
                    {
                      if (ELMA.ID[cx] == ELMA.FAS[ax + 1])
                      {
                        //判断同一电磁阀两端是否同时通电
                        if ((ELMA.state[cx] == ON) && (ELMA.state[y] == ON))  //同时通电，报错并返回
                        {
                          Serial.print("错误！");
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[y]);
                          Serial.print(" 与");
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[cx]);
                          Serial.println("同时通电！");
                          Serial.println("引脚状态写入失败！");
                          break;
                        }
                        else //没有同时通电，继续运行
                        {
                          //不发生冲突时才会将状态发送到对应的引脚
                          if (ELMA.state[y] == 1)
                            pinMode(ELMA.pin[y], HIGH);
                          else
                            pinMode(ELMA.pin[y], LOW);
                          //选择性用
                          Serial.print("电磁铁");
                          Serial.print(ELMA.ID[y]);
                          Serial.println("引脚状态写入成功！");
                        }
                        break;
                      }
                    }
                  }
                }
               if (ELMA.FAD[ax] == ELMA.ID[y]) //为单控电磁阀，执行下列程序
                {
                  if (ELMA.state[y] == 1)
                    pinMode(ELMA.pin[y], HIGH);
                  else
                    pinMode(ELMA.pin[y], LOW);
                  //选择性用
                  Serial.print("电磁铁");
                  Serial.print(ELMA.ID[y]);
                  Serial.println("引脚状态写入成功！");
                  break;
                }
              }
            }
          }
        }
      }
    }
    else   //data[0]=0,修改全部电磁阀状态
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
      Serial.println("20个电磁铁状态为：");
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
      Serial.println();
      for (int y = 0; y < MaxNum; y++)
      {
        //判断电磁铁在哪个电磁阀上，并判断是否电磁阀上两个电磁铁是否同时接通，是则报错，并结束

        for (int ax = 0; ax < MaxNum; ax++) //判断是单控电磁阀还是双控电磁阀
        {
          if (ELMA.FAS[ax] == ELMA.ID[y]) //为双控电磁阀，执行如下程序
          {
            if ((ax % 2) != 0) //y位奇数，但在偶数位
            {
              for (int bx = 0; bx < MaxNum; bx++)
              {
                if (ELMA.ID[bx] == ELMA.FAS[ax - 1])
                {
                  if ((ELMA.state[bx] == ON) && (ELMA.state[y] == ON))
                  {
                    Serial.print("错误！");
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[y]);
                    Serial.print(" 与");
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[bx]);
                    Serial.println("同时通电！");
                    Serial.println("引脚状态写入失败！");
                    break;
                  }
                  else
                  {
                    //不发生冲突时才会将状态发送到对应的引脚
                    if (ELMA.state[y] == 1)
                      pinMode(ELMA.pin[y], HIGH);
                    else
                      pinMode(ELMA.pin[y], LOW);
                    //选择性用
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[y]);
                    Serial.println("引脚状态写入成功！");
                  }
                }
              }
            }
            else  //y为偶数，但在奇数位
            {
              for (int cxa = 0; cxa < MaxNum; cxa++)
              {
                if (ELMA.ID[cxa] == ELMA.FAS[ax + 1])
                {
                  if ((ELMA.state[cxa] == ON) && (ELMA.state[y] == ON))
                  {
                    Serial.print("错误！");
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[y]);
                    Serial.print(" 与");
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[cxa]);
                    Serial.println("同时通电！");
                    Serial.println("引脚状态写入失败！");
                    break;
                  }
                  else
                  {
                    //不发生冲突时才会将状态发送到对应的引脚
                    if (ELMA.state[y] == 1)
                      pinMode(ELMA.pin[y], HIGH);
                    else
                      pinMode(ELMA.pin[y], LOW);
                    //选择性用
                    Serial.print("电磁铁");
                    Serial.print(ELMA.ID[y]);
                    Serial.println("引脚状态写入成功！");
                  }
                }
              }
            }
          }
          else if (ELMA.FAD[ax] == ELMA.ID[y])   //为单控电磁阀，执行下列程序
          {
            if (ELMA.state[y] == 1)
              pinMode(ELMA.pin[y], HIGH);
            else
              pinMode(ELMA.pin[y], LOW);
            //选择性用
            Serial.print("电磁铁");
            Serial.print(ELMA.ID[y]);
            Serial.println("引脚状态写入成功！");
          }
        }
      }
    }
  }
  Serial.println();
  Serial.println("程序执行完成！");
  Serial.println();
}
