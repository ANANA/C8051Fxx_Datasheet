#include <EEPROM.h>

void setup()
{
    Serial.begin(9600);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
    Serial.print("*");
    
  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

void loop()
{
}