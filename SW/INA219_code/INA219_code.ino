#include <Wire.h>
#include <INA219.h>

INA219 INA(0x40);

uint16_t my_shunt= 50; // mohm

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("INA219_LIB_VERSION: ");
  Serial.println(INA219_LIB_VERSION);

  Wire.begin();
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
 // INA.setBusVoltageRange(16);

 pinMode(36, OUTPUT);
 digitalWrite(36, 1);

}






void loop() 
{
  //  these two can be read without further configuration.
  Serial.print(INA.getBusVoltage(), 3);
  Serial.print("\t");
  Serial.print(INA.getShuntVoltage_mV(), 3);
  Serial.println();
  Serial.print("Load current: ");
  Serial.print( INA.getShuntVoltage_mV() * my_shunt /1000);
  Serial.println(" mA ");
  delay(10000);



}
