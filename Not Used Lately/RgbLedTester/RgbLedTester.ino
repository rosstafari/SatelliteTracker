
#include <Wire.h>
 
 
void setup()
{
  Wire.begin(); // set up I2C
  Wire.beginTransmission(0x09);// join I2C, talk to BlinkM 0x09
  Wire.write('o'); // stop script
  delay(5000); // wait

  // now fade to orange
  Wire.write('c'); // ‘c’ == fade to color
  Wire.write(0xff); // value for red channel
  Wire.write(0xa5); // value for blue channel
  Wire.write(0x00); // value for green channel
  Wire.endTransmission(); // leave I2C bus
  
}
 
 
void loop()
{

  delay(5000);           // wait 5 seconds for next scan
}
