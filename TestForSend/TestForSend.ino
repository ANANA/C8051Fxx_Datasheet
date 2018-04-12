#include <mcp2515.h>
#include <SPI.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

void setup()
{
  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;
  canMsg.data[0] = 0x01;
  canMsg.data[1] = 0x03;
  canMsg.data[2] = 0x07;
  canMsg.data[3] = 0x04;
  canMsg.data[4] = 0x0E;
  canMsg.data[5] = 0x02;
  canMsg.data[6] = 0x00;
  canMsg.data[7] = 0x00; //buf[0]!=0
  

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

