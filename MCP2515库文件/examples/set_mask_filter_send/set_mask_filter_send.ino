// demo: set_mask_filter_send
// this demo will show you how to use mask and filter
#include <avr/wdt.h>
#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 6;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(115200);
//wdt_enable(WDTO_4S);//看门狗使能 并设置超时时间
//wdt_disable(); //看门狗禁止
//wdt_reset() ;//看门狗复位
    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void loop()
{
    for(int id=0; id<10; id++)
    {
        memset(stmp, id, sizeof(stmp));                 // set id to send data buff
        CAN.sendMsgBuf(id, 0, sizeof(stmp), stmp);
        delay(100);
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
