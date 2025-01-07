#include <Wire.h>
#include <INA219.h>

INA219 INA(0x40);

float  my_shunt= 50.0f; // mohm

void setup()
{
  Serial.begin(115200);


Wire.begin(2,1);



  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");





  Serial.println(__FILE__);
  Serial.print("INA219_LIB_VERSION: ");
  Serial.println(INA219_LIB_VERSION);

  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
 // INA.setBusVoltageRange(16);

  INA.setMaxCurrentShunt(5, 0.05);
  INA.setGain(2);
  INA.setShuntResolution(12);
  INA.setBusResolution(12);
  INA.setShuntSamples(5);





 pinMode(36, OUTPUT);
 digitalWrite(36, 1);

}






void loop() 
{
  //  these two can be read without further configuration.
  Serial.print("Bus voltage:  ");
  Serial.print(INA.getBusVoltage(), 3);
  Serial.print("\t");
  Serial.print(INA.getShuntVoltage_mV(), 3);
  Serial.println();
  Serial.print("Load current in mA: ");
float shuntVolts = INA.getShuntVoltage_mV();
float loadCurrent =   shuntVolts / ( my_shunt / 1000.0f) ;
Serial.println(loadCurrent, 3);
  Serial.print("Load current in mA: ");
  Serial.println( INA.getCurrent_mA(),3 );
  Serial.println();
  delay(1000);



}


















