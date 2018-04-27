#include <mcp2515.h>
#include <SPI.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

void setup()
{
  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;     //example
  canMsg.data[0] = 0x01;  //0x00           0X01    
  canMsg.data[1] = 0x02;  //FA             0X02
  canMsg.data[2] = 0x06;  //AA             0X00     0x06
  canMsg.data[3] = 0x10;  //AA                                 0x10
  canMsg.data[4] = 0x01;  //                                             0x01
  canMsg.data[5] = 0x14;  //                                                      0x0A                0x14
  canMsg.data[6] = 0x13;  //                                                                0x13      0x13
  canMsg.data[7] = 0xFF;  //              0X80      0xC0       0xE0     0xF0      0xF8      0xFC      0xFF
                       //buf[0]=0  ！=0  只修改1个 只修改2个 只修改3个 只修改4个 只修改5个 只修改6个  自检功能
  

  while (!Serial);
  Serial.begin(115200);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS);
  mcp2515.setNormalMode();

}
void loop()
{
  mcp2515.sendMessage(&canMsg);
  Serial.println("Messages sent OK!!");
  delay(1000);

}

