#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);

void setup()
{
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");


}
unsigned char stmp[8] = {0, 252, 252, 252, 4, 5, 6, 7};  //buf[0]=0
// unsigned char stmp[8] = {1, 252, 252, 252, 4, 5, 6, 7};  //buf[0]=1
// unsigned char stmp[8] = {2, 252, 252, 252, 4, 5, 6, 7};  //buf[0]=2
// unsigned char stmp[8] = {3, 252, 252, 252, 4, 5, 6, 7};  //buf[0]=3
// unsigned char stmp[8] = {3, 3, 252, 252, 4, 5, 6, 7};  //buf[0]=3   测试自我检测功能

void loop()
{
  CAN.sendMsgBuf(0x00, 0, 8, stmp);
  delay(1000);                       // send data once per second
}

